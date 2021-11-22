#ifndef UTILITY_H
#define UTILITY_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Utility {

constexpr int BACKLOG = 20;
constexpr int MAXDATASIZE = 100;

int setNonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

struct NetworkException : std::runtime_error {
  NetworkException(std::string const& msg) : std::runtime_error(msg) {}
  NetworkException(const char* msg) : std::runtime_error(msg) {}
};

void sendStr(std::string const& msg, int fd) {
  // There is no gurantee that the underlying buffer of std::string is
  // continuous.
  std::vector<char> buf{msg.begin(), msg.end()};
  size_t bytes_to_send = buf.size();
  size_t bytes_sent = 0;
  const char* char_array = buf.data();
  while (bytes_to_send > bytes_sent) {
    int bytes_sent_now =
        send(fd, char_array + bytes_sent, bytes_to_send - bytes_sent, 0);
    if (bytes_sent_now == -1) break;
    bytes_sent += bytes_sent_now;
  }
}

std::string recvStr(int fd) {
  static thread_local char buf[3];
  buf[0] = '\0';
  int bytes = recv(fd, buf, MAXDATASIZE - 1, 0);
  if (bytes == -1) return std::string();
  return std::string(buf, bytes);
}

int getSockPort(struct sockaddr* sa) {
  switch (sa->sa_family) {
    case AF_INET:
      return ((struct sockaddr_in*)sa)->sin_port;
    case AF_INET6:
      return ((struct sockaddr_in6*)sa)->sin6_port;
    default:
      return -1;
  }
}

std::string getSockHost(struct sockaddr* sa) {
  thread_local static char host[128];
  host[0] = '\0';
  switch (sa->sa_family) {
    case AF_INET: {
      if (inet_ntop(sa->sa_family, &(((struct sockaddr_in*)sa)->sin_addr), host,
                    sizeof(host)) == NULL) {
        return std::string();
      }
      break;
    }
    case AF_INET6: {
      if (inet_ntop(sa->sa_family, &(((struct sockaddr_in6*)sa)->sin6_addr),
                    host, sizeof(host)) == NULL) {
        return std::string();
      }
      break;
    }
    case AF_UNIX: {
    }
  }
  return std::string(host);
}

int readn(int fd, size_t n, char* buf) {
  size_t nleft = n;
  while (nleft > 0) {
    ssize_t nread = read(fd, buf, nleft);
    if (nread == 0) break;
    nleft -= nread;
    buf += nread;
  }
  return n - nleft;
}

int writen(int fd, size_t n, const char* buf) {
  size_t nleft = n;
  while (nleft > 0) {
    ssize_t nwrite = write(fd, buf, nleft);
    if (nwrite == 0) break;
    nleft -= nwrite;
    buf += nwrite;
  }
  return n - nleft;
}

std::string drain(int fd) {
  char buf[MAXDATASIZE];
  std::string str;
  while (true) {
    int nread = read(fd, buf, MAXDATASIZE - 1);
    if (nread <= 0) {
      break;
    }
    buf[nread] = '\0';
    str += std::string(buf);
  }
  return str;
}

void echo(int fd) {
  char buf[MAXDATASIZE];
  while (true) {
    int nread = read(fd, buf, MAXDATASIZE);
    if (nread < 0) {
      if (errno == EINTR) continue;
      break;
    }
    writen(fd, nread, buf);
  }
}

void echoNonBlock(int fd) { sendStr(recvStr(fd), fd); }

struct SignalHandlerRegistery {
  SignalHandlerRegistery(int signal_number, void (*handler)(int)) {
    assert(signal(signal_number, handler) == 0);
  }
  SignalHandlerRegistery(int signal_number,
                         void (*action)(int, siginfo_t*, void*)) {
    struct sigaction sa = {0};
    sa.sa_flags = signal_number;
    sa.sa_sigaction = action;
    sigemptyset(&sa.sa_mask);
    assert(sigaction(signal_number, &sa, NULL) == 0);
  }
};

};  // namespace Utility
#endif
