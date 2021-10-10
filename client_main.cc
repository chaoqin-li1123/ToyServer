#include <csignal>

#include "client.h"

static Client::Client* client_ptr{nullptr};

static void signalHandler(int signal_number) {
  if (client_ptr) client_ptr->shutdown();
  std::cerr << "client shutdown\n";
}

int main(int argc, char* argv[]) {
  Client::Client client(argv[1], argv[2]);
  client_ptr = &client;
  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);
  client.run();
}