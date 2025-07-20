#include <logWriter.h>
#include <thread>
#include <chrono>

int main() {
    {
        logWriter::socket_logger socket_logger("127.0.0.1", 9001, level::logLevel::DEBUG);
        
        socket_logger.log("This is a debug message", level::logLevel::DEBUG);
        socket_logger.log("This is an info message", level::logLevel::INFO);
        socket_logger.log("This is a critical message", level::logLevel::CRITICAL);
        
        const char* cstr = "C-style string";
        std::string str = "std::string";
        const std::string const_str = "const std::string";
        
        socket_logger.log(cstr);
        socket_logger.log(str);
        socket_logger.log(const_str);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}