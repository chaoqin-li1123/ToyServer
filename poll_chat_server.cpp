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
#include <memory>

#include "utility.h"

using namespace std;


struct Server {
    Server(const char* host_, const char* port_) {
        listener = make_unique<Listener>(host_, port_);
        listener_fd = listener->fd();
        fds = (struct pollfd*)malloc(sizeof *fds * fd_size);
        addFd(listener_fd, POLLIN);
    }
    ~Server() {
        for (int i = 0; i < fd_count; i++) close(fds[i].fd);
        free(fds);
    }
    void addFd(int fd, int events) {
        if (fd_count == fd_size) {
            fd_size *= 2;
            fds = (pollfd*)realloc(fds, fd_size * sizeof(*fds));
        }
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
        listener->startListening();
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
        int fd = listener->acceptConnection();
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
            buf[nbytes] = 0;
            cout << buf;
            send(fd, buf, nbytes, 0);
        }
    }
private:
    std::unique_ptr<Listener> listener;
    int listener_fd{-1};
    set<int> connections;
    int fd_size{5};
    struct pollfd *fds;
    int fd_count{0};
};



int main(int argc, char* argv[]) {
    Server server(argv[1], argv[2]);
    server.run();
}