
#include "logWriter.h"

namespace level {

LOGWRITER_API logLevel str_to_loglevel(const std::string& ll) {
    if (ll == "DEBUG") {
        return logLevel::DEBUG;
    } else if (ll == "INFO") {
        return logLevel::INFO;
    } else if (ll == "CRITICAL") {
        return logLevel::CRITICAL;
    }
    return logLevel::INFO;
}

LOGWRITER_API std::string loglevel_to_str(logLevel ll) {
    switch (ll) {
        case logLevel::DEBUG:
            return "DEBUG";
        case logLevel::INFO:
            return "INFO";
        case logLevel::CRITICAL:
            return "CRITICAL";
    }
    return "INFO";
}

}  // namespace level

namespace details {

LOGWRITER_API std::string get_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    strftime(buf, 20, "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto fraction = now - seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
    return std::string(buf) + "." + std::to_string(milliseconds.count());
}

LOGWRITER_API std::string get_time_for_file() {
    std::time_t t =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[20];
    strftime(buf, 20, "%Y-%m-%d-%H-%M-%S", std::localtime(&t));
    return std::string(buf);
}

}  // namespace details

namespace logWriter {

logger::logger(const std::string& jn, level::logLevel ll)
    : defaultLogLevel(ll) {
    journal.open(jn, std::ios_base::app);
    if (!journal.is_open()) {
        std::cerr << "Error opening file: " << jn << "\n";
    }
}

void logger::setDefaultLogLevel(level::logLevel lvl) { defaultLogLevel.store(lvl); }

level::logLevel logger::getDefaultLogLevel() { return defaultLogLevel; }

void logger::finish() { journal.close();}

template void logger::log<char const*>(char const* msg);

template void logger::log<std::string>(std::string msg);

template void logger::log<std::string&>(std::string& msg);

template void logger::log<const std::string&>(const std::string& msg);

template void logger::log<char const*>(char const* msg, level::logLevel lvl);

template void logger::log<std::string>(std::string msg, level::logLevel lvl);

template void logger::log<std::string&>(std::string& msg, level::logLevel lvl);

template void logger::log<const std::string&>(const std::string& msg,
                                              level::logLevel lvl);

}  // namespace logWriter