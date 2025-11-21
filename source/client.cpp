#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

static const size_t BUFFER_SIZE = 8192;

int send_request(const string &host, int port, const string &request) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Failed to create socket\n";
        return -1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        struct hostent *he = gethostbyname(host.c_str());
        if (!he) {
            cerr << "Failed to resolve host: " << host << "\n";
            close(sock);
            return -1;
        }
        memcpy(&serverAddr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Failed to connect to " << host << ":" << port << "\n";
        close(sock);
        return -1;
    }

    ssize_t sent = write(sock, request.c_str(), request.length());
    if (sent < 0) {
        cerr << "Failed to send request\n";
        close(sock);
        return -1;
    }

    string response;
    char buffer[BUFFER_SIZE];
    ssize_t n;
    while ((n = read(sock, buffer, sizeof(buffer))) > 0) {
        response.append(buffer, buffer + n);
    }

    if (n < 0) {
        cerr << "Error reading response from server\n";
    }

    if (!response.empty()) {
        cout << "Server response:\n" << response << endl;
    } else {
        cout << "(No response from server)\n";
    }

    close(sock);
    return 0;
}

int main(int argc, char *argv[]) {
    string host = "127.0.0.1";
    int port = 8080;

    if (argc > 1) host = argv[1];
    if (argc > 2) port = stoi(argv[2]);

    cout << "OFS Client - Connects to " << host << ":" << port << "\n";
    cout << "Type commands (e.g., LIST /, CREATE /path data owner). Type QUIT to exit." << endl;

    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;
        if (line == "QUIT" || line == "quit" || line == "exit") break;

        int rc = send_request(host, port, line);
        if (rc < 0) {
            cerr << "Request failed\n";
        }
    }

    cout << "Exiting client\n";
    return 0;
}
