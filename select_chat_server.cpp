#include <string>
#include <iostream>
#include <set>
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


struct PollingServer {
    PollingServer() {
        clearFds();
        listener_fd = initListener(NULL, port.c_str());
        FD_SET(listener_fd, &fds);
        resetFdmax();
    }

    ~PollingServer() {
        if (listener_fd != -1) close(listener_fd);
        for (int connection: connections) close(connection); 
    }

    void clearFds() {
        FD_ZERO(&fds);
    }

    void addConnection(int fd) {
        connections.insert(fd);
        FD_SET(fd, &fds);
        resetFdmax();
    }

    void removeConnection(int fd) {
        connections.erase(fd);
        FD_CLR(fd, &fds);
        resetFdmax();
    }


    void resetFdmax() {
        fd_max = listener_fd;
        if (!connections.empty()) fd_max = max(fd_max, *connections.rbegin());
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
            fd_set active = fds; 
            int status = select(fd_max+1, &active, NULL, NULL, NULL);
            if (FD_ISSET(listener_fd, &active)) acceptConnection();
            for (int connection: connections) {
                if (FD_ISSET(connection, &active)) {
                    handleConnection(connection);
                }
            }
        }
    }

    void acceptConnection() {
        cout << "accept connection" << endl;
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen = sizeof remoteaddr;
        int fd = accept(listener_fd, (struct sockaddr *) &remoteaddr, &addrlen);
        if (fd != -1) {
            addConnection(fd);
            string ack{"Connection established.\n"};
            sendStr(ack, fd);
        }        
    }

    void handleConnection(int fd) {
        static char buf[256];
        int nbytes = recv(fd, buf, sizeof buf, 0);
        if (nbytes == 0) {
            close(fd);
            removeConnection(fd);
        }
        else {
            for (int connection: connections) {
                if (connection == fd) continue;
                send(connection, buf, nbytes, 0);
            }
        }
    }



private:
    string port{"10086"};
    int listener_fd{-1};
    set<int> connections;
    fd_set fds;
    int fd_max;
};


int main() {
    PollingServer server;
    server.run();
}