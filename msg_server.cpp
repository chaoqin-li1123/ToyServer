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