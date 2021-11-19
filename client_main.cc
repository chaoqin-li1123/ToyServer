#include <algorithm>
#include <chrono>
#include <csignal>
#include <thread>

#include "client.h"
#include "utility.h"

static Client::Client* client{nullptr};

static void signalHandler(int signal_number) {
  if (client) client->shutdown();
  std::cerr << "client shutdown\n";
  exit(0);
}

static Utility::SignalHandlerRegistery sigterm_registry(SIGTERM, signalHandler);
static Utility::SignalHandlerRegistery sigint_registry(SIGINT, signalHandler);

int main(int argc, char* argv[]) {
  auto retry_interval = std::chrono::seconds(1);
  auto max_retry_interval = std::chrono::seconds(60);
  int retries = 0;
  while (retries <= 15) {
    try {
      client = new Client::Client(argv[1], argv[2]);
      std::cout << "connect successfully" << std::endl;
      break;
    } catch (Utility::NetworkException& e) {
      delete client;
      std::this_thread::sleep_for(retry_interval);
      retry_interval *= 2;
      retry_interval = std::min(retry_interval, max_retry_interval);
      retries++;
      continue;
    }
  }
  if (!client) {
    exit(1);
  }
  client->run();
}