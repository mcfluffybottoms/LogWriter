#include "../include/data_analyzer.hpp"

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: .\\data_analyzer <port> <messages> <time>\n";
    return 1;
  }
  socket_listener::receiver r(std::atoi(argv[1]), std::atoi(argv[2]),
                              std::atoi(argv[3]));
  r.start();
}