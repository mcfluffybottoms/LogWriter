#pragma once

#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#ifdef LOGWRITER_STATIC
#define LOGWRITER_API
#else
#ifdef _WIN32
#ifdef LOGWRITER_EXPORTS
#define LOGWRITER_API __declspec(dllexport)
#else
#define LOGWRITER_API __declspec(dllimport)
#endif
#else
#define LOGWRITER_API
#endif
#endif

namespace level {

enum struct logLevel : int { DEBUG, INFO, CRITICAL };

const std::vector<std::string> logLevelStrings = {"DEBUG", "INFO", "CRITICAL"};

LOGWRITER_API logLevel str_to_loglevel(const std::string &ll);

LOGWRITER_API std::string loglevel_to_str(logLevel ll);

} // namespace level

namespace details {

LOGWRITER_API std::string get_time(std::chrono::system_clock::time_point now);

LOGWRITER_API std::string get_time();

LOGWRITER_API std::string get_time_for_file();

} // namespace details

namespace logWriter {

class LOGWRITER_API base_logger {
  public:
  virtual void log(const std::string &msg, level::logLevel lvl) = 0;
  virtual void log(const std::string &msg) = 0;
  virtual void set_default_loglevel(level::logLevel lvl) = 0;
  virtual level::logLevel get_default_loglevel() const = 0;
  virtual void finish() = 0;
};

class LOGWRITER_API logger : base_logger {
  public:
  logger(const std::string &jn, level::logLevel ll);
  void log(const std::string &msg, level::logLevel lvl) override;
  void log(const std::string &msg) override;
  ~logger();
  void set_default_loglevel(level::logLevel lvl) override;
  level::logLevel get_default_loglevel() const override;
  void finish() override;

  private:
  std::ofstream journal;
  std::mutex m_;
  std::atomic<level::logLevel> defaultLogLevel;
};

class LOGWRITER_API socket_logger : base_logger {
  public:
  socket_logger(const std::string &host, int port, level::logLevel ll);
  void log(const std::string &msg, level::logLevel lvl) override;
  void log(const std::string &msg) override;
  ~socket_logger();
  void set_default_loglevel(level::logLevel lvl) override;
  level::logLevel get_default_loglevel() const override;
  void finish() override;

  private:
  int socket_fd = -1;
  std::mutex m_;
  std::atomic<level::logLevel> defaultLogLevel;
};

} // namespace logWriter