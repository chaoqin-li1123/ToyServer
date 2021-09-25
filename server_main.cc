
#include <iostream>
#include <thread>
#include <vector>

#include "server.h"
#include "utility.h"

int main(int argc, char* argv[]) {
  std::vector<std::thread> workers;
  for (size_t i = 0; i < 5; i++) {
    workers.emplace_back([&, i]() {
      try {
        Server::Server server(argv[1], argv[2]);
        server.run();
      } catch (Utility::NetworkException& e) {
        std::cerr << e.what() << std::endl;
      }
    });
  }
  for (auto& worker : workers) {
    worker.join();
  }
}