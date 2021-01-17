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

struct Listener {
    Listener(std::string const& host, std::string const& port) {
        struct addrinfo *server_info = getServerAddress(NULL, port.c_str());
        struct addrinfo *node;
        int status;
        int yes = 1;
        for (node = server_info; node != NULL; node = node->ai_next) {
            int fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
            listener_fd = fd;
            if (fd == -1) {
                throw NetworkException("server: socket");
                continue;
            }
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
                throw NetworkException("setsockopt");
                exit(1);
            }
            status = bind(fd, node->ai_addr, node->ai_addrlen);
            if (status == -1) {
                close(fd);
                throw NetworkException("server: bind");
                continue;
            }
            break;
        }
        freeaddrinfo(server_info);
    }

    void startListening() {
        if (listener_fd != -1) {
            listen(listener_fd, BACKLOG);
            std::cout << "Start to listen.\n";
        }
        else {
            throw NetworkException("Can't listen on the endpoint.\n");
        }
    }
    int acceptConnection() {
        struct sockaddr_storage their_addr;
        socklen_t sin_size;
        int connection_fd = accept(listener_fd, (struct sockaddr *)&their_addr, &sin_size);
        if (connection_fd == -1) {
            throw NetworkException("accept");
        }
        return connection_fd;
    }
    ~Listener() {
        if (listener_fd != -1) close(listener_fd);
    }
    int fd() const {
        return listener_fd;
    }
private:
    int listener_fd{-1};
    std::string port{"3490"};
};

