
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


// FILE LOGGER

logger::logger(const std::string& jn, level::logLevel ll)
    : defaultLogLevel(ll) {
    journal.open(jn, std::ios_base::app);
    if (!journal.is_open()) {
        std::cerr << "Error opening file: " << jn << "\n";
    }
}

void logger::log(const std::string& msg, level::logLevel lvl) {
    auto time = details::get_time();
    std::string entry = "[" + loglevel_to_str(lvl) + "][" + time + "] " + msg;
    if (lvl < defaultLogLevel) return;
    {
        std::unique_lock<std::mutex> lock(m_);
        journal << entry << std::endl;
    }
}

void logger::log(const std::string& msg) {
    log(msg, defaultLogLevel);
}

logger::~logger() { 
    std::string entry = "Finishing logging...";
    {
        std::unique_lock<std::mutex> lock(m_);
        journal << entry << std::flush;
    }
}

void logger::setDefaultLogLevel(level::logLevel lvl) { defaultLogLevel.store(lvl); }

level::logLevel logger::getDefaultLogLevel() const { return defaultLogLevel; }

void logger::finish() {
    std::string entry = "Finishing logging...";
    {
        std::unique_lock<std::mutex> lock(m_);
        journal << entry << std::endl;
        journal.close();
    }
}

// SOCKET LOGGER

socket_logger::socket_logger(const std::string& host, int port, level::logLevel ll)
    : defaultLogLevel(ll) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Error opening socket.";
        return;
    }
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        close(socket_fd);
        socket_fd = -1;
        return;
    }
    
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(socket_fd);
        socket_fd = -1;
    }

    std::cout << "Connected to " << host << ":" << port << std::endl;
}

void socket_logger::log(const std::string& msg, level::logLevel lvl) {
    auto time = details::get_time();
    std::string entry = "[" + loglevel_to_str(lvl) + "][" + time + "] " + msg + "\n";
    if (lvl < defaultLogLevel) return;
    {
        if (socket_fd < 0) {
            std::cerr << "Socket disconnected.";
            return;
        }
        std::unique_lock<std::mutex> lock(m_);
        if (send(socket_fd, entry.c_str(), entry.size(), 0) < 0) {
            std::cerr << "Issues while sending data. Closing the socket...";
            close(socket_fd);
            socket_fd = -1;
            return;
        }
        fsync(socket_fd);
    }
}

void socket_logger::log(const std::string& msg) {
    log(msg, defaultLogLevel);
}

void socket_logger::setDefaultLogLevel(level::logLevel lvl) { defaultLogLevel.store(lvl); }

level::logLevel socket_logger::getDefaultLogLevel() const { return defaultLogLevel; }

socket_logger::~socket_logger() { 
    finish();
}

void socket_logger::finish() { 
    if(socket_fd > 0) {
        std::string entry = "Closing connection...";
        {
            std::unique_lock<std::mutex> lock(m_);
            if (send(socket_fd, entry.c_str(), entry.size(), MSG_NOSIGNAL) < 0) {
                std::cerr << "Failed to send shutdown message\n";
                return;
            }
            shutdown(socket_fd, SHUT_WR);
        }
        close(socket_fd);
        socket_fd = -1;
    }
}

}  // namespace logWriter