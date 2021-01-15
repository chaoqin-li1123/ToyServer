#include <sys/socket.h>


#include <netdb.h>
#include <arpa/inet.h>

#include <cstring>
#include <iostream>

#define BACKLOG 20
#define MAXDATASIZE 100 

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

int initListener(const char* host, const char* port) {
    struct addrinfo *server_info = getServerAddress(host, port);
    struct addrinfo *node;
    int status;
    int yes = 1;
    int fd = -1;
    for (node = server_info; node != NULL; node = node->ai_next) {
        fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
        if (fd == -1) {
            std::cerr << "server: socket";
            continue;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
            std::cerr << "setsockopt";
            exit(1);
        }
        status = bind(fd, node->ai_addr, node->ai_addrlen);
        if (status == -1) {
            close(fd);
            std::cerr << "server: bind";
            continue;
        }
        break;
    }
    freeaddrinfo(server_info);
    return fd;
}