#include "request_handler.h"
#include "json_handler.h"
#include "session_manager.h"
#include "file_operations.h"
#include <iostream>
using namespace std;

vector<string> RequestHandler::parseCommand(const string& request) {
    vector<string> tokens;
    istringstream stream(request);
    string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

RequestHandler::RequestHandler(OFSInstance& fsInstance) : fs(fsInstance) {}

int RequestHandler::handleFormat(const vector<std::string>& args) {
    if (args.size() < 4) {
        return OFS_ERR_INVALID;
    }
    uint64_t totalSize = stoull(args[1]);
    uint64_t blockSize = stoull(args[2]);
    string diskPath = args[3];
    return fs_format(fs, totalSize, blockSize, diskPath);
}

int RequestHandler::handleInit(const vector<string>& args) {
    if (args.size() < 2) {
        return OFS_ERR_INVALID;
    }
    string diskPath = args[1];
    return fs_init(fs, diskPath);
}

int RequestHandler::handleShutdown(const vector<string>& args) {
    (void)args;
    return fs_shutdown(fs);
}

int RequestHandler::handleCreate(const vector<string>& args) {
    if (args.size() < 4) {
        return OFS_ERR_INVALID;
    }
    string path = args[1];
    string data = args[2];
    string owner = args[3];
    
    int owner_idx = -1;
    if (!fs.userIndex.find(owner, owner_idx)) {
        return OFS_ERR_NOTFOUND;
    }
    
    UserInfo& ownerUser = fs.users[owner_idx];
    int inode = FileOps::file_create(fs, path, data, ownerUser);
    
    return (inode >= 0) ? OFS_SUCCESS : OFS_ERR_INVALID;
}

int RequestHandler::handleDelete(const vector<string>& args) {
    if (args.size() < 3) {
        return OFS_ERR_INVALID;
    }
    string path = args[1];
    string requester = args[2];
    
    int req_idx = -1;
    if (!fs.userIndex.find(requester, req_idx)) {
        return OFS_ERR_INVALID;
    }
    
    UserInfo& requesterUser = fs.users[req_idx];
    bool success = FileOps::file_delete(fs, path, requesterUser);
    
    return success ? OFS_SUCCESS : OFS_ERR_INVALID;
}


int RequestHandler::handleRead(const vector<string>& args) {
    if (args.size() < 3) {
        return OFS_ERR_INVALID;
    }
    string path = args[1];
    string username = args[2];
    
    int user_idx = -1;
    if (!fs.userIndex.find(username, user_idx)) {
        return OFS_ERR_INVALID;
    }
    
    UserInfo& requesterUser = fs.users[user_idx];
    string content = FileOps::file_read(fs, path, requesterUser);
    
    return content.empty() ? OFS_ERR_NOTFOUND : OFS_SUCCESS;
}

int RequestHandler::handleWrite(const vector<std::string>& args) {
    if (args.size() < 3) {
        return OFS_ERR_INVALID;
    }
    string path = args[1];
    string data = args[2];
    
    if (fs.users.empty()) return OFS_ERR_INVALID;
    
    int inode = FileOps::file_create(fs, path, data, fs.users[0]);
    return (inode >= 0) ? OFS_SUCCESS : OFS_ERR_INVALID;
}

int RequestHandler::handleList(const vector<string>& args) {
    string path = (args.size() > 1) ? args[1] : "/";
    string contents = FileOps::dir_list(fs, path);
    
    return contents.empty() && path != "/" ? OFS_ERR_NOTFOUND : OFS_SUCCESS;
}

string RequestHandler::processRequest(const string& request) {
    size_t pos = request.find_first_not_of(" \t\n\r");
    if (pos != string::npos && request[pos] == '{') {
        JSONRequest req = JSONHandler::parseRequest(request);
        return processJsonRequest(req);
    }

    vector<string> tokens = parseCommand(request);

    if (tokens.empty()) {
        return "Empty request";
    }

    string command = tokens[0];
    int result = OFS_ERR_INVALID;

    if (command == "FORMAT") {
        result = handleFormat(tokens);
    } else if (command == "INIT") {
        result = handleInit(tokens);
    } else if (command == "SHUTDOWN") {
        result = handleShutdown(tokens);
    } else if (command == "CREATE") {
        result = handleCreate(tokens);
    } else if (command == "DELETE") {
        result = handleDelete(tokens);
    } else if (command == "READ") {
        result = handleRead(tokens);
    } else if (command == "WRITE") {
        result = handleWrite(tokens);
    } else if (command == "LIST") {
        result = handleList(tokens);
    } else {
        return "Unknown command: " + command;
    }

    if (result == OFS_SUCCESS) {
        return " " + command + " succeeded";
    } else {
        return " " + command + " failed with code " + std::to_string(result);
    }
}

std::string RequestHandler::processJsonRequest(const JSONRequest& req) {
    JSONResponse resp;
    resp.operation = req.operation;
    resp.request_id = req.request_id.empty() ? "" : req.request_id;
    resp.status = "success";

    auto get_param = [&](const std::string& key) -> std::string {
        auto it = req.parameters.find(key);
        return (it == req.parameters.end()) ? std::string("") : it->second;
    };

    if (req.operation == "FORMAT") {
        std::string totalSizeStr = get_param("total_size");
        std::string blockSizeStr = get_param("block_size");
        std::string path = get_param("disk_path");
        if (totalSizeStr.empty() || blockSizeStr.empty() || path.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Missing parameters for FORMAT";
            return JSONHandler::generateResponse(resp);
        }
        uint64_t totalSize = std::stoull(totalSizeStr);
        uint64_t blockSize = std::stoull(blockSizeStr);
        int rc = fs_format(fs, totalSize, blockSize, path);
        if (rc != OFS_SUCCESS) {
            resp.status = "error";
            resp.error_code = rc;
            resp.error_message = "Formatting failed";
        } else {
            resp.data["message"] = "Formatted";
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "INIT") {
        std::string path = get_param("disk_path");
        if (path.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Missing disk_path for INIT";
            return JSONHandler::generateResponse(resp);
        }
        int rc = fs_init(fs, path);
        if (rc != OFS_SUCCESS) {
            resp.status = "error";
            resp.error_code = rc;
            resp.error_message = "Init failed";
        } else {
            resp.data["message"] = "Initialized";
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "SHUTDOWN") {
        int rc = fs_shutdown(fs);
        if (rc != OFS_SUCCESS) {
            resp.status = "error";
            resp.error_code = rc;
            resp.error_message = "Shutdown failed";
        } else {
            resp.data["message"] = "Shutdown";
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "CREATE") {
        std::string path = get_param("path");
        std::string data = get_param("data");
        std::string owner = get_param("owner");
        if (path.empty() || owner.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Missing parameters for CREATE";
            return JSONHandler::generateResponse(resp);
        }
        int idx = -1;
        if (!fs.userIndex.find(owner, idx)) {
            resp.status = "error";
            resp.error_code = OFS_ERR_NOTFOUND;
            resp.error_message = "Owner not found";
            return JSONHandler::generateResponse(resp);
        }
        UserInfo &ownerUser = fs.users[idx];
        int inode = FileOps::file_create(fs, path, data, ownerUser);
        if (inode < 0) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "File create failed";
        } else {
            resp.data["inode"] = std::to_string(inode);
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "DELETE") {
        std::string path = get_param("path");
        std::string requester = get_param("user");
        if (path.empty() || requester.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Missing parameters for DELETE";
            return JSONHandler::generateResponse(resp);
        }
        int req_idx = -1;
        if (!fs.userIndex.find(requester, req_idx)) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Requester not found";
            return JSONHandler::generateResponse(resp);
        }
        UserInfo &requesterUser = fs.users[req_idx];
        bool success = FileOps::file_delete(fs, path, requesterUser);
        if (!success) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Delete failed";
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "READ") {
        std::string path = get_param("path");
        std::string username = get_param("user");
        if (path.empty() || username.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Missing parameters for READ";
            return JSONHandler::generateResponse(resp);
        }
        int user_idx = -1;
        if (!fs.userIndex.find(username, user_idx)) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "User not found";
            return JSONHandler::generateResponse(resp);
        }
        UserInfo &requesterUser = fs.users[user_idx];
        std::string content = FileOps::file_read(fs, path, requesterUser);
        if (content.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_NOTFOUND;
            resp.error_message = "File not found or permission denied";
        } else {
            resp.data["content"] = content;
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "EDIT") {
        std::string path = get_param("path");
        std::string new_data = get_param("data");
        std::string username = get_param("user");
        if (path.empty() || username.empty()) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "Missing parameters for EDIT";
            return JSONHandler::generateResponse(resp);
        }
        int user_idx = -1;
        if (!fs.userIndex.find(username, user_idx)) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "User not found";
            return JSONHandler::generateResponse(resp);
        }
        UserInfo &requesterUser = fs.users[user_idx];
        bool success = FileOps::file_edit(fs, path, new_data, requesterUser);
        if (!success) {
            resp.status = "error";
            resp.error_code = OFS_ERR_INVALID;
            resp.error_message = "File edit failed or permission denied";
        } else {
            resp.data["message"] = "File edited successfully";
        }
        return JSONHandler::generateResponse(resp);
    } else if (req.operation == "LIST") {
        std::string path = get_param("path");
        if (path.empty()) path = "/";
    std::string contents = FileOps::dir_list(fs, path);
    if (contents.empty() && path != "/") {
            resp.status = "error";
            resp.error_code = OFS_ERR_NOTFOUND;
            resp.error_message = "Directory not found";
        } else {
            resp.data["listing"] = contents;
        }
        return JSONHandler::generateResponse(resp);
    }

    resp.status = "error";
    resp.error_code = OFS_ERR_INVALID;
    resp.error_message = "Unknown operation";
    return JSONHandler::generateResponse(resp);
}
