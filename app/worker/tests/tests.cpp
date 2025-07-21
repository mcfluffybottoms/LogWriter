#include <testFramework.h>
#include "../include/worker.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <array>
#include <vector>

namespace testSpace {

/*
    Helps to ignore times used.
*/
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

/*
    Function to compare actual data with expected data.
*/
bool comparefiles(std::vector<std::string>& f1, const std::string& p2) {
    std::ifstream f2(p2, std::ifstream::binary);

    if (f1.empty() || f2.fail()) {
        std::cout << "f1.fail() || f2.fail()\n";
        return false;
    }

    std::string f2_line;
    size_t line_number = 0;
    while (std::getline(f2, f2_line)) {
        ++line_number;
        if (f1.size() < line_number || !linesEqual(f2_line, f1[line_number - 1])) {
            std::cerr << "      Wrong line " << line_number << "\n";
            std::cerr << "      1st file: " << f1[line_number - 1] << "\n";
            std::cerr << "      2st file: " << f2_line << "\n";
            return false;
        }
    }
    ++line_number;
    if(f1.size() > line_number) {
        std::cerr << "      Wrong line " << line_number << "\n";
        std::cerr << "      1st file: " << f1[line_number - 1] << "\n";
        std::cerr << "      2st file: " << "" << "\n";
        return false;
    }
    return true;
}

/*
    Class with very long stream operator loading.
*/
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

}

using namespace testSpace;

int main() {
    auto& runner = TestRunner::getInstance();
    std::vector<std::string> expected;
    
    //-----------TESTING LOGWORKER-----------//
    // TEST 1
    expected = {
        "[DEBUG][...] LOGGER MESSAGE 1",
        "",
    };
    runner.runTest("Simple log message",
        [&runner, &expected](){
            if(std::filesystem::exists("TEMPlogWorkerTest-1")) {
                std::remove("TEMPlogWorkerTest-1");
            }
            app::logWorker::worker worker("TEMPlogWorkerTest-1", "DEBUG");
            worker.log("LOGGER MESSAGE 1");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            runner.assertTrue(comparefiles(expected, "TEMPlogWorkerTest-1"));
            std::remove("TEMPlogWorkerTest-1");
        }
    );
    // TEST 2
    expected = {
        "[DEBUG][...] LOGGER MESSAGE 1",
        "[DEBUG][...] LOGGER MESSAGE 2",
        "[DEBUG][...] LOGGER MESSAGE 3",
        "[DEBUG][...] LOGGER MESSAGE 4",
        "[DEBUG][...] LOGGER MESSAGE 5",
        ""
    };
    runner.runTest("Simple log message with multiple threads",
        [&runner, &expected](){
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
            runner.assertTrue(comparefiles(expected, "TEMPlogWorkerTest-2"));
            //std::remove("TEMPlogWorkerTest-2");
        }
    );
    // TEST 3
    expected = {
        "[CRITICAL][...] LOGGER MESSAGE 3",
        "[CRITICAL][...] LOGGER MESSAGE 6",
        ""
    };
    runner.runTest("Simple log message with multiple threads and different log levels",
        [&runner, &expected](){
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
            runner.assertTrue(comparefiles(expected, "TEMPlogWorkerTest-3"));
            std::remove("TEMPlogWorkerTest-3");
        }
    );

}
