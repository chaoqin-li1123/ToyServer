#include "utility.h"

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
        socklen_t sin_size = sizeof their_addr;
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