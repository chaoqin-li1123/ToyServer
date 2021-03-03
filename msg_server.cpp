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
    Server(const char* host_, const char* port_) {
        listener = make_unique<Listener>(host_, port_);
    }
    void run() {
        listener->startListening();
        while (true) {
            int connection_fd = listener->acceptConnection();
            if (connection_fd != -1) {
                int pid = fork();
                // handle a new connection in a child process.
                if (pid == 0) {
                    echo(connection_fd);
                }   
                else {
                    children.push_back(pid);
                }             
            }
        }
    }
    ~Server() {
        // for (int child: children) kill(child, SIGKILL);
    }
    
private:
    unique_ptr<Listener> listener;
    // list<Connection> connections;
    list<int> children;
};


int main(int argc, char* argv[]) {
    Server server(argv[1], argv[2]);
    server.run();
}