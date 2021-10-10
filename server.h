#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <thread>
#include <vector>

#include "listener.h"
#include "utility.h"

using ::Utility::NetworkException;

namespace Server {

struct Server {
  explicit Server(const char* port_) : Server(nullptr, port_) {}
  Server(const char* host_, const char* port_) {
    listener = std::make_unique<Listener::Listener>(host_, port_);
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

  void run() {
    while (true) {
      if (is_shutdown) return;
      int events_count = epoll_wait(epoll_fd, events, epoll_size, 100);
      if (events_count == -1) throw NetworkException("epoll wait");
      if (events_count > 0) processEvents(events_count);
    }
  }

  void shutdown() { is_shutdown = true; }

 private:
  void handleConnection(int fd) {
    std::string recv_msg = Utility::recvStr(fd);
    if (recv_msg.empty()) {
      // If a readable socket has nothing to read, then it is closed.
      close(fd);
      std::cout << "Connection closed\n";
    } else {
      Utility::sendStr(recv_msg, fd);
    }
  }

  void handleConnectionByFork(int fd) {
    int pid = fork();
    // handle connection in the child process.
    if (pid == 0) {
      Utility::echoNonBlock(fd);
      exit(0);
    }
  }

  void addFd(int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    Utility::setNonblocking(fd);
  }

  void processEvents(int events_count) {
    for (int i = 0; i < events_count; i++) {
      int fd = events[i].data.fd;
      if (fd == listener_fd) {
        int connection = listener->acceptConnection();
        addFd(connection);
      } else if (events[i].events & EPOLLIN) {
        handleConnection(fd);
      }
    }
  }

  int epoll_fd{-1};
  static constexpr int epoll_size{10};
  epoll_event events[epoll_size];
  int listener_fd{-1};
  std::unique_ptr<Listener::Listener> listener;
  bool is_shutdown{false};
};

struct ServerManager {
  void registerServer(Server& server) {
    std::unique_lock<std::mutex> lck(register_mtx);
    servers.push_back(&server);
  }

  void shutdown() {
    std::unique_lock<std::mutex> lck(register_mtx);
    for (Server* server : servers) server->shutdown();
  }

  static ServerManager& getServerManager() {
    static ServerManager server_manager;
    return server_manager;
  }

 private:
  ServerManager() {}
  std::vector<Server*> servers;
  std::mutex register_mtx;
};

};  // namespace Server
#endif