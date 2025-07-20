#include <logWriter.h>
#include <iostream>
#include <stdexcept>
#include <string>

struct simpleStream {
    simpleStream(int v) : value(v) {}
    friend std::ostream& operator<<(std::ostream& os, const simpleStream& obj);
private:
    int value;
};

std::ostream& operator<<(std::ostream& os, const simpleStream& obj)
{
    os << "simpleStream object has " << obj.value;
    return os;
}

struct testStruct {
    testStruct(int i, const std::string& s) : value_int(i), value_string(s), streamObj(i) {}

    int throwError() {
        throw std::runtime_error("Got RunmtimeError!");
    }
    int returnInteger() {
        return value_int;
    }
    const std::string& returnString() {
        return value_string;
    }
    const simpleStream& returnSimpleStream() {
        return streamObj;
    }
private:
    int value_int;
    std::string value_string;
    simpleStream streamObj;
};

// int main() {
//     std::string ll;
//     std::cout << "Enter Default Log Level: " << "\n";
//     std::getline(std::cin, ll);

//     std::string fileName = "log-" + details::get_time_for_file() + ".txt";
//     logWriter::logger logManager(fileName, level::str_to_loglevel(ll));
//     try {
//         logManager.log("Initializing testStruct with data 1 and \"string\". ", level::logLevel::DEBUG);
//         testStruct test(1, "string");
//         logManager.log("Running returnInteger(). ", level::logLevel::DEBUG);
//         logManager.log(test.returnInteger(), level::logLevel::INFO);
//         logManager.log("Running returnString(). ", level::logLevel::DEBUG);
//         logManager.log(test.returnString(), level::logLevel::INFO);
//         //logManager.log("Running returnSimpleStream(). ", level::logLevel::DEBUG);
//         //logManager.log(test.returnSimpleStream(), level::logLevel::INFO);
//         logManager.log("Running throwError(). ", level::logLevel::INFO);
//         logManager.log(test.throwError(), level::logLevel::INFO);
//     } catch(std::exception& e) {
//         logManager.log(e.what(), level::logLevel::CRITICAL);
//     }
// }

int main() {
    std::string ll;
    std::cout << "Enter Default Log Level: " << "\n";
    std::getline(std::cin, ll);

    std::string fileName = "log-" + details::get_time_for_file() + ".txt";
    logWriter::logger logManager(fileName, level::str_to_loglevel(ll));
    try {
        logManager.log("Initializing testStruct with data 1 and \"string\". ", level::logLevel::DEBUG);
        testStruct test(1, "string");
        logManager.log("Running returnString(). ", level::logLevel::DEBUG);
        logManager.log(test.returnString(), level::logLevel::INFO);
        logManager.log("Running throwError(). ", level::logLevel::INFO);
        logManager.log(std::to_string(test.throwError()), level::logLevel::INFO);
    } catch(std::exception& e) {
        logManager.log(e.what(), level::logLevel::CRITICAL);
    }
}