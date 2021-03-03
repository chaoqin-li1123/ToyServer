#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>

#include "utility.h"

using namespace std;

struct Connection {
    Connection(string ip_, string port_): ip{ip_}, port{port_}, fd{-1}
    {   
        buildConnection();
        if (fd != -1) run();
    }

    void buildConnection() {
        struct addrinfo * server_info = getClientAddress(ip.c_str(), port.c_str());
        struct addrinfo * node;
        int status = -1;
        for (node = server_info; node != NULL; node = node->ai_next) {
            fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
            if (fd == -1) {
                cerr << "fail to create socket";
                continue;
            }
            status = connect(fd, node->ai_addr, node->ai_addrlen);
            if (status == -1) {
                cerr << "client fail to connect.\n";
                continue;
            }
            break;
        }
    }

    void run() {
        char buf[MAXDATASIZE];
        string line;
        // hit ctrl D to exit the loop
        while (getline(cin, line)) {
            writen(fd, line.size(), line.c_str());
            int nread = read(fd, buf, MAXDATASIZE);
            buf[nread] = '\0';
            cout << buf << endl;
        }
        close(fd);
    }

    ~Connection() {
        if (fd != -1) close(fd);
    }





private:
    string port;
    string ip;
    int fd;
};

struct Client {
    Client(string ip_, string port_): connection(ip_, port_) {
    }
private:
    Connection connection;
};

int main(int argc, char* argv[]) {
    Client client(argv[1], argv[2]);

}