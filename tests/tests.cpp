#include <testFramework.h>
#include "../include/worker.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <array>

namespace testSpace {

bool linesEqual(std::string& l1, std::string& l2) {
    auto l1_start = l1.find("]");
    auto l2_start = l2.find("]");
    auto l1_end = l1.find("]", l1_start + 1);
    auto l2_end = l2.find("]", l2_start + 1);
    if (l1_start != std::string::npos && l1_end != std::string::npos) {
        l1.replace(l1_start + 1, l1_end - l1_start, "[]");
    }
    if (l2_start != std::string::npos && l2_end != std::string::npos) {
        l2.replace(l2_start + 1, l2_end - l2_start, "[]");
    }
    return l1 == l2;
}

bool comparefiles(const std::string& p1, const std::string& p2) {
    std::ifstream f1(p1, std::ifstream::binary);
    std::ifstream f2(p2, std::ifstream::binary);

    if (f1.fail() || f2.fail()) {
        std::cout << "f1.fail() || f2.fail()\n";
        return false;
    }

    std::string f1_line;
    std::string f2_line;
    size_t line_number = 0;
    while (std::getline(f1, f1_line)) {
        ++line_number;
        if (!std::getline(f2, f2_line) || !linesEqual(f1_line, f2_line)) {
            std::cerr << "      Wrong line " << line_number << "\n";
            std::cerr << "      1st file: " << f1_line << "\n";
            std::cerr << "      2st file: " << f2_line << "\n";
            return false;
        }
    }
    if(std::getline(f2, f2_line)) {
        std::cout << f2_line << "\n";
        return false;
    }
    return true;
}

struct longStream {
    longStream(int v) : value(v) {}
    friend std::ostream& operator<<(std::ostream& os, const longStream& obj);
    int value;
};

std::ostream& operator<<(std::ostream& os, const longStream& obj)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20*obj.value));
    os << "LOGGER MESSAGE " << obj.value;
    return os;
}

std::string slow_function(int i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return "LOGGER MESSAGE " + i;
}

}

using namespace testSpace;

int main() {
    auto& runner = TestRunner::getInstance();
    
    // TESTING LOGWORKER
    runner.runTest("Simple log message",
        [&runner](){
            if(std::filesystem::exists("TEMPlogWorkerTest-1")) {
                std::remove("TEMPlogWorkerTest-1");
            }
            app::logWorker::worker worker("TEMPlogWorkerTest-1", "DEBUG");
            worker.log("LOGGER MESSAGE 1");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            runner.assertTrue(comparefiles(RESOURCES_DIR "\\appExpectedLogs\\1", "TEMPlogWorkerTest-1"));
            std::remove("TEMPlogWorkerTest-1");
        }
    );
    runner.runTest("Simple log message with multiple threads",
        [&runner](){
            if(std::filesystem::exists("TEMPlogWorkerTest-2")) {
                std::remove("TEMPlogWorkerTest-2");
            }
            app::logWorker::worker worker("TEMPlogWorkerTest-2", "DEBUG", 5);
            std::array<longStream, 5> streams = {
                longStream(1), longStream(2), longStream(3), 
                longStream(4), longStream(5)
            };
            for (auto& stream : streams) {
                worker.log(stream);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            runner.assertTrue(comparefiles(RESOURCES_DIR "\\appExpectedLogs\\2", "TEMPlogWorkerTest-2"));
            std::remove("TEMPlogWorkerTest-2");
        }
    );
    runner.runTest("Simple log message with multiple threads and different log levels",
        [&runner](){
            if(std::filesystem::exists("TEMPlogWorkerTest-3")) {
                std::remove("TEMPlogWorkerTest-3");
            }
            app::logWorker::worker worker("TEMPlogWorkerTest-3", "CRITICAL", 5);
            std::array<longStream, 6> streams = {
                longStream(1), longStream(2), longStream(3), 
                longStream(4), longStream(5), longStream(6)
            };
            std::string data[] = {"DEBUG", "INFO", "CRITICAL"};
            for (int i = 0 ; i < 6; i++) {
                worker.log(streams[i], data[i % 3]);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            runner.assertTrue(comparefiles(RESOURCES_DIR "\\appExpectedLogs\\3", "TEMPlogWorkerTest-3"));
            std::remove("TEMPlogWorkerTest-3");
        }
    );

}
