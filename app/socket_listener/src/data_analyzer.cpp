#include "../include/data_analyzer.hpp"

namespace socket_listener {

void dataCollector::get_data(char* buffer) {
    if (num_of_msg > 0) {
        avrg_len = sum_len / num_of_msg;
    }

    snprintf(buffer, 256,
        "Total messages: %zu\n"
        "DEBUG: %zu\n"
        "INFO: %zu\n"
        "CRITICAL: %zu\n"
        "Messages in last hour: %zu\n"
        "Min length: %zu\n"
        "Max length: %zu\n"
        "Average length: %zu",
        num_of_msg.load(),
        num_DEBUG.load(),
        num_INFO.load(),
        num_CRIT.load(),
        logs_lasthour.size(),
        min_len.load(),
        max_len.load(),
        avrg_len.load()
    );
}
void dataCollector::cleanup(std::chrono::system_clock::time_point now) {
    while (!logs_lasthour.empty() && logs_lasthour.front() < now - std::chrono::hours(1)) {
        logs_lasthour.pop();
    }
}

receiver::receiver(int port, size_t N, size_t T) : interval_messages(N), interval(T) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Error opening socket.";
        running_ = false;
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        close(socket_fd);
        socket_fd = -1;
        running_ = false;
        return;
    }
    if (listen(socket_fd, SOMAXCONN ) < 0) {
        std::cerr << "Listen failed." << std::endl;
        close(socket_fd);
        socket_fd = -1;
        running_ = false;
        return;
    }
    main_loop_thread_ = std::thread(&receiver::start, this);
    statistics_thread_ = std::thread(&receiver::stats, this);
}

receiver::~receiver() {
    running_ = false;
    if (main_loop_thread_.joinable()) main_loop_thread_.join();
    if (statistics_thread_.joinable()) statistics_thread_.join();
}

void receiver::start() {
    if (socket_fd < 0) {
        std::cerr << "Socket disconnected.\n";
        return;
    }

    while (running_) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(socket_fd, (sockaddr*)&client_addr, &len);
        if (client_fd < 0) {
            std::cerr << "Accept failed.\n";
            continue;
        }

        std::string buffer_accum;
        char buffer[BUFFER_SIZE];

        while (running_) {
            ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0) {
                buffer[n] = '\0';
                buffer_accum.append(buffer);
            } else if (n == 0) {
                break;
            } else {
                perror("recv");
                break;
            }
        }

        if (!buffer_accum.empty()) {
            handle_request(buffer_accum.data());
        }

        close(client_fd);

        if (collector.num_of_msg % interval_messages == 0 && running_) {
            std::unique_lock<std::mutex> lock(m_);
            std::cout << "-------- REPORT AFTER " << interval_messages << " MESSAGES --------\n";
            char report_buf[256] = {0};
            collector.cleanup(std::chrono::system_clock::now());
            collector.get_data(report_buf);
            std::cout << report_buf << "\n";
        }
    }

    close(socket_fd);
}

void receiver::stats() {
    if (socket_fd < 0) {
        std::cerr << "Socket disconnected.\n";
        return;
    }
    while(running_) {
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(interval));
        char buffer[256] = {0};
        if(running_) {
            std::unique_lock<std::mutex> lock(m_);
            collector.cleanup(std::chrono::system_clock::now());
            std::cout << "-------- REPORT AFTER " << interval << " MS --------\n";
            collector.get_data(buffer);
            std::cout << buffer << "\n";
        }
    }
}

void receiver::handle_request(char* data) {
    std::unique_lock<std::mutex> lock(m_);
    while(data && *data) {
        // time
        char* level_start = strchr(data, '[');
        if (!level_start) return;
        char* level_end = strchr(level_start + 1, ']');
        if (!level_end) return;
        find_level(data);
        // level
        char* time_start = strchr(level_end + 1, '[');
        if (!time_start) return;
        char* time_end = strchr(time_start + 1, ']');
        if (!time_end) return;
        // messages
        char* message_start = time_end + 1;
        if (!message_start) return;
        char* message_end = strchr(message_start, '\n');
        if (!message_end) {
            message_end = message_start + strlen(message_start);
        }
        size_t len = message_end - message_start - 1;
        data = message_end + 1;
        // count messages
        ++collector.num_of_msg;
        // filter length
        if(collector.max_len < len) {
            collector.max_len = len;
        }
        if(collector.min_len > len) {
            collector.min_len = len;
        }
        collector.sum_len += len;
        collector.avrg_len += collector.sum_len / collector.num_of_msg;
        // per hour
        char buf[20] = {0}; 
        strncpy(buf, time_start + 1, time_end - time_start);
        buf[19] = '\0';
        const auto time = find_time(buf);
        collector.logs_lasthour.push(time);
        collector.cleanup(std::chrono::system_clock::now());
    }
}

void receiver::find_level(char* str) {
    if(strncmp("[CRITICAL]", str, strlen("[CRITICAL]")) == 0) {
        ++collector.num_CRIT;
    } else if(strncmp("[DEBUG]", str, strlen("[DEBUG]")) == 0) {
        ++collector.num_DEBUG;
    } else {
        ++collector.num_INFO;
    }
}

std::chrono::system_clock::time_point receiver::find_time(char* t_str) {
    struct tm time = {};
    strptime(t_str, "%Y-%m-%d %H:%M:%S", &time);
    std::time_t tt = std::mktime(&time);
    return std::chrono::system_clock::from_time_t(tt);
}

}