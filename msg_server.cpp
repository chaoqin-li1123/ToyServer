#include <string>
#include <iostream>
#include <set>
#include <map>
#include <memory>
#include <thread>
#include <list>

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>


#include <netdb.h>
#include <arpa/inet.h>

#include "utility.h"



using namespace std;

struct Listener {
    Listener() {
        struct addrinfo *server_info = getServerAddress(NULL, port.c_str());
        struct addrinfo *node;
        int status;
        int yes = 1;
        for (node = server_info; node != NULL; node = node->ai_next) {
            int fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
            listener_fd = fd;
            if (fd == -1) {
                cerr << "server: socket";
                continue;
            }
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
                cerr << "setsockopt";
                exit(1);
            }
            status = bind(fd, node->ai_addr, node->ai_addrlen);
            if (status == -1) {
                close(fd);
                cerr << "server: bind";
                continue;
            }
            break;
        }
        freeaddrinfo(server_info);
    }

    void startListening() {
        if (listener_fd != -1) {
            listen(listener_fd, BACKLOG);
            cout << "Start to listen.\n";
        }
        else {
            cerr << "Can't listen on the endpoint.\n";
        }
    }
    int acceptConnection() {
        struct sockaddr_storage their_addr;
        socklen_t sin_size;
        int connection_fd = accept(listener_fd, (struct sockaddr *)&their_addr, &sin_size);
        if (connection_fd == -1) {
            cerr << "accept";
        }
        return connection_fd;
    }
    ~Listener() {
        if (listener_fd != -1) close(listener_fd);
    }
private:
    int listener_fd{-1};
    string port{"3490"};
};

void handleConnection(int connection_fd) {
    sendStr("connection established\n", connection_fd);
}

struct Connection {
    Connection(int connection_fd_): connection_fd{connection_fd_}, 
    worker{handleConnection, connection_fd_} {}
    ~Connection() {
        worker.join();
    }
private:
    int connection_fd;
    thread worker;
};

struct Server {
    Server() {
        listener = make_unique<Listener>();
    }
    void run() {
        listener->startListening();
        while (true) {
            int connection_fd = listener->acceptConnection();
            if (connection_fd != -1) {
                cout << "accept connection\n";
                connections.emplace_back(connection_fd);
            }
        }
    }
    ~Server() {
    }
    
private:
    unique_ptr<Listener> listener;
    list<Connection> connections;
};


int main() {
    Server server;
    server.run();
}