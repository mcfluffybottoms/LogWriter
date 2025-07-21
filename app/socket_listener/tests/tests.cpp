#include "../include/data_analyzer.hpp"
#include <chrono>
#include <cstring>
#include <logWriter.h>
#include <testFramework.h>

struct dummyReceiver {
  socket_listener::dataCollector collector{};

  void find_level(char *str) {
    if (strncmp("[CRITICAL]", str, strlen("[CRITICAL]")) == 0) {
      ++collector.num_CRIT;
    } else if (strncmp("[DEBUG]", str, strlen("[DEBUG]")) == 0) {
      ++collector.num_DEBUG;
    } else {
      ++collector.num_INFO;
    }
  }

  std::chrono::system_clock::time_point find_time(char *t_str) {
    struct tm time = {};
    strptime(t_str, "%Y-%m-%d %H:%M:%S", &time);
    std::time_t tt = std::mktime(&time);
    return std::chrono::system_clock::from_time_t(tt);
  }

  // functions from receiver
  void handle_request(char *data) {
    while (data && *data) {
      // time
      char *level_start = strchr(data, '[');
      if (!level_start)
        return;
      char *level_end = strchr(level_start + 1, ']');
      if (!level_end)
        return;
      find_level(data);
      // level
      char *time_start = strchr(level_end + 1, '[');
      if (!time_start)
        return;
      char *time_end = strchr(time_start + 1, ']');
      if (!time_end)
        return;
      // messages
      char *message_start = time_end + 1;
      if (!message_start)
        return;
      char *message_end = strchr(message_start, '\n');
      if (!message_end) {
        message_end = message_start + strlen(message_start);
      }
      size_t len = message_end - message_start - 1;
      data = message_end + 1;
      // count messages
      ++collector.num_of_msg;
      // filter length
      if (collector.max_len < len) {
        collector.max_len = len;
      }
      if (collector.min_len > len) {
        collector.min_len = len;
      }
      collector.sum_len += len;
      collector.avrg_len = collector.sum_len / collector.num_of_msg;
      // per hour
      char buf[20] = {0};
      strncpy(buf, time_start + 1, time_end - time_start);
      buf[19] = '\0';
      const auto time = find_time(buf);
      collector.logs_lasthour.push(time);
      collector.cleanup(std::chrono::system_clock::now());
    }
  }
};

int main() {
  auto &runner = TestRunner::getInstance();

  // TEST 1
  runner.runTest("Test handle_task", [&runner]() {
    char input_data[] = "[DEBUG][2024-07-20 17:58:13.41] LOGGER MESSAGE "
                        "1\n[INFO][2024-07-20 17:58:13.41] LOGGER MESSAGE 2\n"
                        "[CRITICAL][2024-07-20 17:58:13.41] LOGGER MESSAGE 3\n"
                        "[DEBUG][2024-07-20 17:58:13.41] LOGGER MESSAGE 4\n"
                        "[INFO][2024-07-20 17:58:13.41] LOGGER MESSAGE 5\n"
                        "[CRITICAL][2024-07-20 17:58:13.41] LOGGER MESSAGE 6\n";
    dummyReceiver r;
    r.handle_request(input_data);
    runner.assertEqual(r.collector.num_of_msg.load(), size_t(6),
                       "Failed for num_of_msg");
    runner.assertEqual(r.collector.num_DEBUG.load(), size_t(2),
                       "Failed for num_DEBUG");
    runner.assertEqual(r.collector.num_INFO.load(), size_t(2),
                       "Failed for num_INFO");
    runner.assertEqual(r.collector.num_CRIT.load(), size_t(2),
                       "Failed for num_CRIT");
    runner.assertEqual(r.collector.logs_lasthour.size(), size_t(0),
                       "Failed for logs_lasthour");
    runner.assertEqual(r.collector.min_len.load(), size_t(16),
                       "Failed for min_len");
    runner.assertEqual(r.collector.max_len.load(), size_t(16),
                       "Failed for max_len");
    runner.assertEqual(r.collector.avrg_len.load(), size_t(16),
                       "Failed for avrg_len");
    runner.assertEqual(r.collector.sum_len.load(), size_t(96),
                       "Failed for sum_len");
  });

  // TEST 2
  runner.runTest("Test find_time", [&runner]() {
    dummyReceiver r;
    const auto now = std::chrono::system_clock::now();
    auto input = details::get_time(now);
    const auto output = r.find_time(input.data());

    constexpr auto tolerance = std::chrono::milliseconds(1000); // on account of milliseconds
    const auto diff =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - output);
    const auto abs_diff = std::abs(diff.count());

    runner.assertTrue(abs_diff < tolerance.count(),
                      "Time difference exceeds tolerance: (" +
                          std::to_string(abs_diff) + "ms)");
  });

  // TEST 3
  runner.runTest("Test find_level", [&runner]() {
    dummyReceiver r;
    char debug_msg[] = "[DEBUG][2024-07-20 17:58:13.41] LOGGER MESSAGE ";
    char info_msg[] = "[INFO][2024-07-20 17:58:13.41] LOGGER MESSAGE ";
    char critical_msg[] = "[CRITICAL][2024-07-20 17:58:13.41]";
    r.find_level(debug_msg);
    r.find_level(info_msg);
    r.find_level(critical_msg);
    runner.assertEqual(r.collector.num_DEBUG.load(), size_t(1));
    runner.assertEqual(r.collector.num_INFO.load(), size_t(1));
    runner.assertEqual(r.collector.num_CRIT.load(), size_t(1));
  });
}
