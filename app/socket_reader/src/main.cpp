#include "../include/data_analyzer.hpp"

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cerr << "Usage: .\\socket_reader <port> <messages> <time>\n";
        return 1;
    }
    socket_reader::receiver r(std::atoi(argv[1]), std::atoi(argv[2]), std::atoi(argv[3]));
    r.start();
}