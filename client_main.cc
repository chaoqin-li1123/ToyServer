#include <chrono>
#include <csignal>
#include <thread>

#include "client.h"
#include "utility.h"

static Client::Client* client_ptr{nullptr};

static void signalHandler(int signal_number) {
  if (client_ptr) client_ptr->shutdown();
  std::cerr << "client shutdown\n";
}

int main(int argc, char* argv[]) {
  auto retry_interval = std::chrono::microseconds(1000);
  int retries = 0;
  while (retries < 6) {
    try {
      client_ptr = new Client::Client(argv[1], argv[2]);
      std::cout << "connect successfully" << std::endl;
      break;
    } catch (Utility::NetworkException& e) {
      delete client_ptr;
      std::this_thread::sleep_for(retry_interval);
      retry_interval *= 2;
      continue;
    }
  }
  if (!client_ptr) {
    exit(1);
  }
  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);
  client_ptr->run();
}