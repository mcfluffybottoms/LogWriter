#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

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

LOGWRITER_API logLevel str_to_loglevel(const std::string& ll);

LOGWRITER_API std::string loglevel_to_str(logLevel ll);

}  // namespace level

namespace details {

LOGWRITER_API std::string get_time();

LOGWRITER_API std::string get_time_for_file();

}  // namespace details

namespace logWriter {

class LOGWRITER_API base_logger {
   public:
    virtual void log(const std::string& msg, level::logLevel lvl) = 0;
    virtual void log(const std::string& msg) = 0;
    virtual void setDefaultLogLevel(level::logLevel lvl) = 0;
    virtual level::logLevel getDefaultLogLevel() const = 0;
    virtual void finish() = 0;
};

class LOGWRITER_API logger : base_logger {
   public:
    logger(const std::string& jn, level::logLevel ll);
    void log(const std::string& msg, level::logLevel lvl) override;
    void log(const std::string& msg) override;
    ~logger();
    void setDefaultLogLevel(level::logLevel lvl) override;
    level::logLevel getDefaultLogLevel() const override;
    void finish() override;

   private:
    std::ofstream journal;
    std::mutex m_;
    std::atomic<level::logLevel> defaultLogLevel;
};

class LOGWRITER_API socket_logger : base_logger {
   public:
    socket_logger(const std::string& host, int port, level::logLevel ll);
    void log(const std::string& msg, level::logLevel lvl) override;
    void log(const std::string& msg) override;
    ~socket_logger();
    void setDefaultLogLevel(level::logLevel lvl) override;
    level::logLevel getDefaultLogLevel() const override;
    void finish() override;

   private:
    int socket_fd = -1;
    std::mutex m_;
    std::atomic<level::logLevel> defaultLogLevel;
};

}  // namespace logWriter