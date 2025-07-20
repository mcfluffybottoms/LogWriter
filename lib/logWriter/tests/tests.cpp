#include <testFramework.h>
#include <logWriter.h>
#include <regex>
#include <string>
#include <algorithm>


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

}

using namespace testSpace;

void test_log() {
    auto& runner = TestRunner::getInstance();
    std::vector<std::string> expected;

    expected = {
        "[DEBUG][...] LOGGER MESSAGE 1",
        "",
    };
    runner.runTest("Simple log message",
        [&runner, &expected](){
            if(std::filesystem::exists("TEMPlogWriterTest-1")) {
                std::remove("TEMPlogWriterTest-1");
            }
            logWriter::logger logger("TEMPlogWriterTest-1", level::logLevel::DEBUG);
            logger.log("LOGGER MESSAGE 1");
            runner.assertTrue(comparefiles(expected, "TEMPlogWriterTest-1"));
            logger.finish();
            std::remove("TEMPlogWriterTest-1");
        }
    );
    expected = {
        "[DEBUG][...] LOGGER MESSAGE 1",
        "[INFO][...] LOGGER MESSAGE 2",
        "[CRITICAL][...] LOGGER MESSAGE 3",
        "",
    };
    runner.runTest("Several Inputs",
        [&runner, &expected](){
            if(std::filesystem::exists("TEMPlogWriterTest-2")) {
                std::remove("TEMPlogWriterTest-2");
            }
            logWriter::logger logger("TEMPlogWriterTest-2", level::logLevel::DEBUG);
            logger.log("LOGGER MESSAGE 1");
            logger.log("LOGGER MESSAGE 2", level::logLevel::INFO);
            logger.log("LOGGER MESSAGE 3", level::logLevel::CRITICAL);
            runner.assertTrue(comparefiles(expected, "TEMPlogWriterTest-2"));
            logger.finish();
            std::remove("TEMPlogWriterTest-2");
        }
    );
    expected = {
        "[CRITICAL][...] LOGGER MESSAGE 3",
        "[CRITICAL][...] LOGGER MESSAGE 4",
        "",
    };
    runner.runTest("Filters Out Logging Levels",
        [&runner, &expected](){
            if(std::filesystem::exists("TEMPlogWriterTest-3")) {
                std::remove("TEMPlogWriterTest-3");
            }
            logWriter::logger logger("TEMPlogWriterTest-3", level::logLevel::CRITICAL);
            logger.log("LOGGER MESSAGE 1", level::logLevel::DEBUG);
            logger.log("LOGGER MESSAGE 2", level::logLevel::INFO);
            logger.log("LOGGER MESSAGE 3", level::logLevel::CRITICAL);
            logger.log("LOGGER MESSAGE 4");
            runner.assertTrue(comparefiles(expected, "TEMPlogWriterTest-3"));
            logger.finish();
            std::remove("TEMPlogWriterTest-3");
        }
    );
    expected = {
        "[INFO][...] LOGGER MESSAGE 2",
        "[CRITICAL][...] LOGGER MESSAGE 3",
        "[CRITICAL][...] LOGGER MESSAGE 3",
        "[DEBUG][...] LOGGER MESSAGE 1",
        "[INFO][...] LOGGER MESSAGE 2",
        "[CRITICAL][...] LOGGER MESSAGE 3",
        "",
    };
    runner.runTest("Simple Change Default Logging Levels",
        [&runner, &expected](){
            if(std::filesystem::exists("TEMPlogWriterTest-4")) {
                std::remove("TEMPlogWriterTest-4");
            }
            // INITIALLY SET TO INFO
            logWriter::logger logger("TEMPlogWriterTest-4", level::logLevel::INFO);
            logger.log("LOGGER MESSAGE 1", level::logLevel::DEBUG);
            logger.log("LOGGER MESSAGE 2", level::logLevel::INFO);
            logger.log("LOGGER MESSAGE 3", level::logLevel::CRITICAL);
            // CHANGE TO CRITICAL
            logger.setDefaultLogLevel(level::logLevel::CRITICAL);
            runner.assertEqual(level::logLevel::CRITICAL, logger.getDefaultLogLevel());
            logger.log("LOGGER MESSAGE 1", level::logLevel::DEBUG);
            logger.log("LOGGER MESSAGE 2", level::logLevel::INFO);
            logger.log("LOGGER MESSAGE 3", level::logLevel::CRITICAL);
            // CHANGE TO DEBUG
            logger.setDefaultLogLevel(level::logLevel::DEBUG);
            runner.assertEqual(level::logLevel::DEBUG, logger.getDefaultLogLevel());
            logger.log("LOGGER MESSAGE 1", level::logLevel::DEBUG);
            logger.log("LOGGER MESSAGE 2", level::logLevel::INFO);
            logger.log("LOGGER MESSAGE 3", level::logLevel::CRITICAL);
            // CHECK RESULT
            runner.assertTrue(comparefiles(expected, "TEMPlogWriterTest-4"));
            logger.finish();
            std::remove("TEMPlogWriterTest-4");
        }
    );
}

int main() {
    test_log();
}