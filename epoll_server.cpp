
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <iostream>
#include <set>
#include <memory>

#include "utility.h"

using namespace std;

int setNonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

struct Server {
    Server(const char* host_, const char* port_) {
        listener = make_unique<Listener>(host_, port_);
        listener_fd = listener->fd();
        listener->startListening();
        epoll_fd = epoll_create(5);
        if (epoll_fd == -1) throw NetworkException("epoll create");
        addFd(listener_fd);
    }

    ~Server() {
        for (int i = 0; i < epoll_size; i++) {
            int fd = events[i].data.fd;
            if (fd != listener_fd) close(fd);
        }
    }

    void addFd(int fd) {
        epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN;
        if (et) event.events |= EPOLLET;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
        setNonblocking(fd); 
    }

    void lt(int events_count) {
        for (int i = 0; i < events_count; i++) {
            int fd = events[i].data.fd;
            if (fd == listener_fd) {
                int connection = listener->acceptConnection();
                addFd(connection);
                sendStr("connection established\n", connection);
            }
            else if (events[i].events & EPOLLIN) {
                handleConnection(fd);
            }
        }
    }

    void run() {
        while (true) {
            int events_count = epoll_wait(epoll_fd, events, epoll_size, -1);
            if (events_count == -1) throw NetworkException("epoll wait");
            lt(events_count);
        }
    }

    void clearBuf() {
        memset(buf, '\0', MAXDATASIZE);
    }

    void handleConnection(int fd) {
        clearBuf();
        int nbytes = recv(fd, buf, sizeof buf, 0);
        cout << buf;
        send(fd, buf, nbytes, 0);
    }

    
private:
    int epoll_fd{-1};
    static constexpr int epoll_size{10};
    epoll_event events[epoll_size];
    int listener_fd{-1};
    bool et{false};
    unique_ptr<Listener> listener;
    char buf[MAXDATASIZE];
};


int main(int argc, char* argv[]) {
    Server server(argv[1], argv[2]);
    server.run();
}