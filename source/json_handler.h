#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <string>
#include <map>
#include <sstream>
#include <iostream>
using namespace std;


struct JSONRequest {
    string operation;
    string session_id;
    string request_id;
    map<string, string> parameters;
};

struct JSONResponse {
    string status;  
    string operation;
    string request_id;
    int error_code = 0;
    string error_message;
    map<string, string> data;
};

class JSONHandler {
public:
   
    static string escapeString(const string& s) {
        string out;
        for (char c : s) {
            switch (c) {
                case '\\': out += "\\\\"; break;
                case '"': out += "\\\""; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default:
                    if ((unsigned char)c < 0x20) {
                        char buf[8];
                        snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                        out += buf;
                    } else {
                        out += c;
                    }
                    break;
            }
        }
        return out;
    }
    static JSONRequest parseRequest(const string& json) {
        JSONRequest req;
        
        size_t op_pos = json.find("\"operation\"");
        if (op_pos != string::npos) {
            size_t start = json.find("\"", op_pos + 12) + 1;
            size_t end = json.find("\"", start);
            req.operation = json.substr(start, end - start);
        }
        
        size_t sid_pos = json.find("\"session_id\"");
        if (sid_pos != string::npos) {
            size_t start = json.find("\"", sid_pos + 13) + 1;
            size_t end = json.find("\"", start);
            req.session_id = json.substr(start, end - start);
        }
        
        size_t rid_pos = json.find("\"request_id\"");
        if (rid_pos != string::npos) {
            size_t start = json.find("\"", rid_pos + 13) + 1;
            size_t end = json.find("\"", start);
            req.request_id = json.substr(start, end - start);
        }
        
        size_t param_pos = json.find("\"parameters\"");
        if (param_pos != string::npos) {
            size_t start = json.find("{", param_pos);
            size_t end = json.find("}", start);
            string params_str = json.substr(start + 1, end - start - 1);
            
            size_t pos = 0;
            while (pos < params_str.length()) {
                size_t key_start = params_str.find("\"", pos);
                if (key_start == string::npos) break;
                size_t key_end = params_str.find("\"", key_start + 1);
                string key = params_str.substr(key_start + 1, key_end - key_start - 1);
                
                size_t colon = params_str.find(":", key_end);
                size_t val_start = params_str.find("\"", colon);
                size_t val_end = params_str.find("\"", val_start + 1);
                string value = params_str.substr(val_start + 1, val_end - val_start - 1);
                
                req.parameters[key] = value;
                pos = val_end + 1;
            }
        }
        
        return req;
    }

    static string generateResponse(const JSONResponse& resp) {
        stringstream ss;
        ss << "{\"status\":\"" << resp.status << "\","
           << "\"operation\":\"" << resp.operation << "\","
           << "\"request_id\":\"" << resp.request_id << "\"";
        
        if (resp.status == "error") {
            ss << ",\"error_code\":" << resp.error_code
               << ",\"error_message\":\"" << escapeString(resp.error_message) << "\"";
        } else {
            ss << ",\"data\":{";
            bool first = true;
            for (const auto& kv : resp.data) {
                if (!first) ss << ",";
                ss << "\"" << kv.first << "\":\"" << escapeString(kv.second) << "\"";
                first = false;
            }
            ss << "}";
        }
        
        ss << "}";
        return ss.str();
    }
};

#endif
