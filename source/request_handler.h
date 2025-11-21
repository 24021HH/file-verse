#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "ofs_core.h"
#include "json_handler.h"
#include <string>
#include <sstream>
#include <map>
using namespace std;


class RequestHandler {
private:
    OFSInstance& fs;

    vector<string> parseCommand(const string& request);

    int handleFormat(const vector<string>& args);
    int handleInit(const vector<string>& args);
    int handleShutdown(const vector<string>& args);
    int handleCreate(const vector<string>& args);
    int handleDelete(const vector<string>& args);
    int handleRead(const vector<string>& args);
    int handleWrite(const vector<string>& args);
    int handleList(const vector<string>& args);

public:
    RequestHandler(OFSInstance& fsInstance);

    string processRequest(const string& request);
    string processJsonRequest(const JSONRequest& req);
};

#endif
