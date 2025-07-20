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

class LOGWRITER_API logger {
   public:
    logger(const std::string& jn, level::logLevel ll);

    template <typename T>
    void log(T msg, level::logLevel lvl) {
        auto time = details::get_time();
        if (lvl < defaultLogLevel) return;
        {
            std::unique_lock<std::mutex> lock(m_);
            journal << "[" << loglevel_to_str(lvl) << "][" << time << "] " << msg
                << std::endl;
        }
        
    }

    template <typename T>
    void log(T msg) {
        log(std::forward<T>(msg), defaultLogLevel);
    }

    void setDefaultLogLevel(level::logLevel lvl);

    level::logLevel getDefaultLogLevel();

    void finish();

   private:
    std::ofstream journal;
    std::mutex m_;
    std::atomic<level::logLevel> defaultLogLevel;  // make it atomic
};

extern template LOGWRITER_API void logger::log<char const*>(char const* msg);

extern template LOGWRITER_API void logger::log<std::string>(std::string msg);

extern template LOGWRITER_API void logger::log<std::string&>(std::string& msg);

extern template LOGWRITER_API void logger::log<const std::string&>(
    const std::string& msg);

extern template LOGWRITER_API void logger::log<char const*>(
    char const* msg, level::logLevel lvl);

extern template LOGWRITER_API void logger::log<std::string>(
    std::string msg, level::logLevel lvl);

extern template LOGWRITER_API void logger::log<std::string&>(
    std::string& msg, level::logLevel lvl);

extern template LOGWRITER_API void logger::log<const std::string&>(
    const std::string& msg, level::logLevel lvl);

}  // namespace logWriter