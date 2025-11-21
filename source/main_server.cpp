#include "server.h"
#include "request_handler.h"
#include "ofs_core.h"
#include <iostream>
#include <thread>
#include <signal.h>
using namespace std;
Server* globalServer = nullptr;

void signalHandler(int signum) {
    cout << "\nReceived signal " << signum << ", shutting down\n";
    if (globalServer) {
        globalServer->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    cout << "Server Implementation \n\n";


    int port = 8080;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    OFSInstance fs;
    std::cout << "Initializing filesystem instance...\n";
    fs_format(fs, 10 * 1024 * 1024, 4096, "fs_server.omni");
    fs_init(fs, "fs_server.omni");

    RequestHandler handler(fs);

    Server server(port, &handler);
    globalServer = &server;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "Starting OFS server on port " << port << "...\n\n";

    server.run();

    fs_shutdown(fs);
    std::cout << "Server shutdown complete\n";

    return 0;
}
