#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "utility.h"

namespace Client {

struct addrinfo* getClientAddress(const char* host, const char* port) {
  struct addrinfo hints, *server_info;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(host, port, &hints, &server_info);
  return server_info;
}

struct ClientConnection {
  ClientConnection(std::string ip_, std::string port_)
      : ip{ip_}, port{port_}, fd{-1} {
    buildConnection();
    if (fd != -1) run();
  }

  void buildConnection() {
    struct addrinfo* server_info = getClientAddress(ip.c_str(), port.c_str());
    struct addrinfo* node;
    int status = -1;
    for (node = server_info; node != NULL; node = node->ai_next) {
      fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
      if (fd == -1) {
        std::cerr << "fail to create socket";
        continue;
      }
      status = connect(fd, node->ai_addr, node->ai_addrlen);
      if (status == -1) {
        std::cerr << "client fail to connect.\n";
        continue;
      }
      break;
    }
  }

  void run() {
    Utility::setNonblocking(0);
    Utility::setNonblocking(fd);
    std::string line;
    std::cout << "start client main loop\n";
    while (true) {
      std::string recv_msg = Utility::recvStr(fd);
      if (!recv_msg.empty()) {
        std::cout << recv_msg;
      }

      std::string input = Utility::drain(STDIN_FILENO);
      if (!input.empty()) {
        Utility::sendStr(input, fd);
      }
    }
    close(fd);
  }

  ~ClientConnection() {
    if (fd != -1) close(fd);
  }

 private:
  std::string ip;
  std::string port;
  int fd;
};

struct Client {
  Client(std::string ip_, std::string port_) : connection(ip_, port_) {}

 private:
  ClientConnection connection;
};

};  // namespace Client

#endif