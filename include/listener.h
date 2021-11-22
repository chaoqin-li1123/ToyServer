#ifndef LISTENER_H
#define LISTENER_H
#include "utility.h"

using ::Utility::NetworkException;

namespace Listener {

static struct addrinfo *getServerAddress(const char *host, const char *port) {
  struct addrinfo hints, *server_info;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(host, port, &hints, &server_info) != 0) {
    throw NetworkException("getaddrinfo failure.");
  }
  return server_info;
}

struct Listener {
  explicit Listener(const char *port_) : Listener(nullptr, port_) {}
  Listener(const char *host_, const char *port_) {
    struct addrinfo *server_info = getServerAddress(host_, port_);
    struct addrinfo *node;
    int status;
    int yes = 1;
    for (node = server_info; node != NULL; node = node->ai_next) {
      int fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
      listener_fd = fd;
      if (fd == -1) {
        throw NetworkException("Create socket failure.");
        continue;
      }
      if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes,
                     sizeof(int)) == -1) {
        throw NetworkException("setsockopt failure.");
        exit(1);
      }
      status = bind(fd, node->ai_addr, node->ai_addrlen);
      if (status == -1) {
        close(fd);
        throw NetworkException("Bind socket failure.");
        continue;
      }
      break;
    }
    freeaddrinfo(server_info);
  }

  void startListening() {
    if (listener_fd != -1) {
      listen(listener_fd, Utility::BACKLOG);
      std::cout << "Start to listen.\n";
    } else {
      throw NetworkException("Can't listen on the endpoint.\n");
    }
  }
  int acceptConnection() {
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof their_addr;
    int connection_fd =
        accept(listener_fd, (struct sockaddr *)&their_addr, &sin_size);
    if (connection_fd == -1) {
      throw NetworkException("accept");
    }
    return connection_fd;
  }
  ~Listener() {
    if (listener_fd != -1) close(listener_fd);
  }
  int fd() const { return listener_fd; }

 private:
  int listener_fd{-1};
  std::string port{"3490"};
};

};  // namespace Listener
#endif