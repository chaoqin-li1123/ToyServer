#ifndef UTILITY
#define UTILITY

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

int get_sock_port(struct sockaddr * sa) {
    switch(sa->sa_family) {
        case AF_INET:
            return ((struct sockaddr_in*) sa)->sin_port;
        case AF_INET6:
            return ((struct sockaddr_in6*) sa)->sin6_port;
        default:
            return -1;
    }
}


std::string get_sock_host(struct sockaddr * sa) {
    char host[128];
    host[0] = '\0';
    switch(sa->sa_family) {
        case AF_INET: {
            if (inet_ntop(sa->sa_family, &(((struct sockaddr_in *)sa)->sin_addr), host, sizeof(host)) == NULL) {
                return std::string();
            }
            break;
        }
        case AF_INET6: {
            if (inet_ntop(sa->sa_family, &(((struct sockaddr_in6 *)sa)->sin6_addr), host, sizeof(host)) == NULL) {
                return std::string();
            }
            break;
        }
        case AF_UNIX: {

        }
    }
    return std::string(host);
}


int readn(int fd, size_t n, char* buf) {
    size_t nleft = n;
    while (nleft > 0) {
        ssize_t nread = read(fd, buf, nleft);
        if (nread == 0) break;
        nleft -= nread;
        buf += nread;
    }
    return n - nleft;
}

int writen(int fd, size_t n, const char* buf) {
    size_t nleft = n;
    while (nleft > 0) {
        ssize_t nwrite = write(fd, buf, nleft);
        if (nwrite == 0) break;
        nleft -= nwrite;
        buf += nwrite;
    } 
    return n - nleft;
}

void echo(int fd) {
    char buf[MAXDATASIZE];
    while (true) {
        int nread = read(fd, buf, MAXDATASIZE);
        if (nread < 0) {
            if (errno == EINTR) continue;
            break; 
        }
        writen(fd, nread, buf);
    }
}

void echo_nonblock(int fd) {
    char buf[MAXDATASIZE];
    int nread;
    while ((nread = read(fd, buf, MAXDATASIZE)) != 0) {
        if (nread < 0) {
            if (errno == EINTR) continue;
            break; 
        }
        writen(fd, nread, buf);
    }
}

#endif

