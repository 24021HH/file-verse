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

    // Default values
    std::string diskPath = "ofs.omni";
    int port = 8080;
    
    // Parse command line arguments
    if (argc > 1) {
        diskPath = argv[1];
    }
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    OFSInstance fs;
    std::cout << "Initializing filesystem from: " << diskPath << "\n";
    
    // Initialize existing filesystem (don't format)
    int result = fs_init(fs, diskPath);
    if (result != OFS_SUCCESS) {
        std::cerr << "Failed to initialize filesystem. Please run FORMAT first.\n";
        return 1;
    }

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
