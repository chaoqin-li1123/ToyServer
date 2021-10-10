
#include <csignal>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "server.h"
#include "utility.h"

static void signalHandler(int signal_number) {
  Server::ServerManager::getServerManager().shutdown();
  std::cerr << "server shutdown\n";
}

int main(int argc, char* argv[]) {
  std::vector<std::thread> workers;
  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);
  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
    workers.emplace_back([&, i]() {
      try {
        Server::Server server(argv[1], argv[2]);
        Server::ServerManager::getServerManager().registerServer(server);
        server.run();
        std::cerr << "worker exit\n";
      } catch (Utility::NetworkException& e) {
        std::cerr << e.what() << std::endl;
      }
    });
  }
  for (auto& worker : workers) {
    worker.join();
  }
}