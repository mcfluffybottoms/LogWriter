#include <chrono>
#include <logWriter.h>
#include <thread>

int main() {
  {
    logWriter::socket_logger socket_logger("127.0.0.1", 9001,
                                           level::logLevel::DEBUG);

    socket_logger.log("This is a debug message", level::logLevel::DEBUG);
    socket_logger.log("This is an info message", level::logLevel::INFO);
    socket_logger.log("This is a critical message", level::logLevel::CRITICAL);
    socket_logger.log("This is a debug message", level::logLevel::DEBUG);
    socket_logger.log("This is an info message", level::logLevel::INFO);
    socket_logger.log("This is a critical message", level::logLevel::CRITICAL);
    socket_logger.log("This is a debug message", level::logLevel::DEBUG);
    socket_logger.log("This is an info message", level::logLevel::INFO);
    socket_logger.log("This is a critical message", level::logLevel::CRITICAL);
    socket_logger.log("This is a debug message", level::logLevel::DEBUG);
    socket_logger.log("This is an info message", level::logLevel::INFO);
    socket_logger.log("This is a critical message", level::logLevel::CRITICAL);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    socket_logger.finish();
  }

  return 0;
}