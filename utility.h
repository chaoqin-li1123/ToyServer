#include <sys/socket.h>


#include <netdb.h>
#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <stdexcept>

#define BACKLOG 20
#define MAXDATASIZE 100 

struct NetworkException: std::runtime_error {
    NetworkException(std::string const& msg): std::runtime_error(msg) {}
    NetworkException(const char * msg): std::runtime_error(msg) {}
};

struct addrinfo* getServerAddress(const char* host, const char* port) {
    struct addrinfo hints, *server_info;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(host, port, &hints, &server_info);
    return server_info;
}

struct addrinfo* getClientAddress(const char* host, const char* port) {
    struct addrinfo hints, *server_info;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host, port, &hints, &server_info);
    return server_info;
}

void sendStr(std::string const& msg, int fd) {
    send(fd, msg.c_str(), msg.size(), 0);
}

std::string recvStr(int fd) {
    char buf[MAXDATASIZE];
    int bytes = recv(fd, buf, MAXDATASIZE-1, 0);
    if (bytes == -1) return std::string();
    buf[bytes] = '\0';
    return std::string(buf);
}



