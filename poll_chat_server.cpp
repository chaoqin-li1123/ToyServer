#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <poll.h>
#include <arpa/inet.h>

#include <iostream>
#include <set>

#include "utility.h"

using namespace std;


struct Server {
    Server() {
        listener_fd = initListener(NULL, port.c_str());
        fds = (struct pollfd*)malloc(sizeof *fds * fd_size);
        addFd(listener_fd, POLLIN);
    }
    ~Server() {
        for (int i = 0; i < fd_count; i++) close(fds[i].fd);
        free(fds);
    }
    void addFd(int fd, int events) {
        fds[fd_count].fd = fd;
        fds[fd_count].events = events;
        fd_count++; 
    }

    void removeFd(int fd) {
        int i;
        for (i = 0; i < fd_count; i++) {
            if (fds[i].fd == fd) break;
        }
        swap(fds[i], fds[fd_count]);
        fd_count--;;
    }
    
    void run() {
        int status = listen(listener_fd, BACKLOG);
        if (status != -1) {
            cout << "start listening" << endl;
        }
        else {
            cerr << "fail to listen\n";
            return;
        }
        while (true) {
            int poll_count = poll(fds, fd_count, -1);
            for (int i = 0; i < fd_count; i++) {
                if (fds[i].revents & POLLIN == 0) {
                    // NO event to handle.
                    continue;
                }
                if (fds[i].fd == listener_fd) {
                    acceptConnection();
                }
                else {
                    handleConnection(fds[i].fd);
                }
            }
        }
    }

    void acceptConnection() {
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen = sizeof remoteaddr;
        int fd = accept(listener_fd, (struct sockaddr *) &remoteaddr, &addrlen);
        if (fd != -1) {
            addFd(fd, POLLIN);
            string ack{"Connection established.\n"};
            sendStr(ack, fd);
        }        
    }

    void handleConnection(int fd) {
        static char buf[256];
        int nbytes = recv(fd, buf, sizeof buf, 0);
        if (nbytes == 0) {
            close(fd);
            removeFd(fd);
        }
        else {
            for (int connection: connections) {
                if (connection == fd) continue;
                send(connection, buf, nbytes, 0);
            }
        }
    }
private:
    string port{"2077"};
    int listener_fd{-1};
    set<int> connections;
    int fd_size{5};
    struct pollfd *fds;
    int fd_count{0};
};



int main() {
    Server server;
    server.run();
}