#pragma once
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace socket_listener {

struct dataCollector {
  std::atomic<size_t> num_of_msg{0};
  std::atomic<size_t> num_DEBUG{0};
  std::atomic<size_t> num_INFO{0};
  std::atomic<size_t> num_CRIT{0};
  std::queue<std::chrono::system_clock::time_point> logs_lasthour;

  std::atomic<size_t> min_len = -1;
  std::atomic<size_t> max_len{0};
  std::atomic<size_t> avrg_len{0};
  std::atomic<size_t> sum_len{0}; // for avrg_len

  void get_data(char *buffer);
  void cleanup(std::chrono::system_clock::time_point now);
};

struct receiver {

  receiver(int port, size_t N, size_t T);

  ~receiver();

  void start();

  void stats();

  void handle_request(char *data);

  private:
  size_t interval_messages;
  size_t interval;
  std::thread main_loop_thread_;
  std::thread statistics_thread_;
  std::mutex m_;
  dataCollector collector{};
  bool data_changed = false;
  static inline std::atomic<bool> running_{true};
  int socket_fd = -1;
  const int BUFFER_SIZE = 4096;

  void find_level(char *str);

  std::chrono::system_clock::time_point find_time(char *t_str);
};

} // namespace socket_listener