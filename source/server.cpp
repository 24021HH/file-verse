#include "server.h"
#include "request_handler.h"
#include <thread>
#include <iostream>

Server::Server(int p, RequestHandler* handlerPtr) : serverSocket(-1), port(p), isRunning(false), handler(handlerPtr) {}


Server::~Server() {
    if (serverSocket != -1) {
        close(serverSocket);
    }
}

bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options\n";
        close(serverSocket);
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(port);

    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind socket to port " << port << "\n";
        close(serverSocket);
        return false;
    }

    if (::listen(serverSocket, MAX_BACKLOG) < 0) {
        std::cerr << "Failed to listen on socket\n";
        close(serverSocket);
        return false;
    }

    isRunning = true;
    std::cout << "Server listening on port " << port << "\n";
    return true;
}

void Server::stop() {
    isRunning = false;
    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }
    std::cout << "Server stopped\n";
}

void Server::handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0};

    int bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead < 0) {
        std::cerr << "Failed to read from client\n";
        close(clientSocket);
        return;
    }

    std::string request(buffer, bytesRead);
    std::cout << " Received request: " << request << "\n";

    std::string response;
    if (handler) {
        response = handler->processRequest(request);
    } else {
        response = "Request received: " + request;
    }
    write(clientSocket, response.c_str(), response.length());

    close(clientSocket);
    std::cout << " Client connection closed\n";
}

void Server::run() {
    if (!start()) {
        return;
    }

    std::cout << "Server running. Press Ctrl+C to stop.\n";

    while (isRunning) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (isRunning) {
                std::cerr << "Failed to accept client connection\n";
            }
            break;
        }

        std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << "\n";


        handleClient(clientSocket);
    }

    stop();
}
