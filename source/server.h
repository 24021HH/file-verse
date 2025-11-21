#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using namespace std;


class Server {
private:
    int serverSocket;
    int port;
    bool isRunning;
    class RequestHandler* handler;

    static const int MAX_BACKLOG = 5;
    static const int BUFFER_SIZE = 4096;

public:
    Server(int p = 8080, class RequestHandler* handlerPtr = nullptr);
    void setRequestHandler(class RequestHandler* handlerPtr) { handler = handlerPtr; }


    ~Server();

    bool start();

    void stop();

    void handleClient(int clientSocket);

    void run();

    int getPort() const { return port; }

    bool isServerRunning() const { return isRunning; }
};

#endif 
