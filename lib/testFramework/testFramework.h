#include <iostream>
#include <string>
#include <functional>
#include <exception>
#include <queue>


class TestRunner {
public:
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    void runTest(const std::string& test_name, std::function<void()> test_func) {
        std::cout << "Running test: " << test_name << "...\n";
        current_test_failures = 0;
        
        try {
            test_func();
            if (current_test_failures == 0) {
                passed++;
                std::cout << "PASSED.\n";
            } else {
                failed++;
                std::cout << "FAILED (" << current_test_failures << " assertions failed).\n";
            }
        } 
        catch (const std::exception& e) {
            failed++;
            current_test_failures++;
            std::cout << "FAILED (Exception: " << e.what() << ").\n";
        } 
        catch (...) {
            failed++;
            current_test_failures++;
            std::cout << "FAILED (Unknown exception).\n";
        }
    }

    template <typename T>
    bool assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            current_test_failures++;
            std::cerr << "\n  Assertion failed. " << message << "\n";
            return false;
        }
        return true;
    }

    bool assertTrue(bool condition, const std::string& message = "") {
        return assertEqual(true, condition, message);
    }

    bool assertFalse(bool condition, const std::string& message = "") {
        return assertEqual(false, condition, message);
    }

    template <typename ExceptionType, typename Func, typename... Args>
    void assertThrows(Func func, Args... args, const std::string& message = "") {
        try {
            func(args...);
            current_test_failures++;
            std::cerr << "\n  Assertion failed: " << message
                      << "\n  Expected exception: " << typeid(ExceptionType).name()
                      << "\n  But no exception was thrown\n";
        } catch (const ExceptionType&) {
        } catch (...) {
            current_test_failures++;
            std::cerr << "\n  Assertion failed: " << message
                      << "\n  Wrong exception type thrown\n";
        }
    }

    bool printSummary() const {
        std::cout << "\n===== Test Summary =====\n";
        std::cout << "Total tests: " << (passed + failed) << "\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n\n";
        return failed == 0;
    }

private:
    TestRunner() = default;
    TestRunner(const TestRunner&) = delete;
    TestRunner& operator=(const TestRunner&) = delete;

    int passed = 0;
    int failed = 0;
    int current_test_failures = 0;
};
