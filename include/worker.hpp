#include <logWriter.h>
#include <mutex>
#include <thread>
#include <memory>
#include <cstddef>
#include <string>
#include <queue>
#include <condition_variable>

namespace app {
		
namespace logWorker {

    class worker {
    public:
        worker(const std::string& filename, const std::string& default_loglevel, size_t number_of_threads = 1);

        template <typename T>
        void log(T&& msg, const std::string& ll) {
            {
                std::unique_lock<std::mutex> lock(m_);
                q.push(std::make_unique<task<T>>(std::forward<T>(msg), level::str_to_loglevel(ll)));
            }
            cv_.notify_one();
        }

        template <typename T>
        void log(T&& msg) {
            {
                std::unique_lock<std::mutex> lock(m_);
                q.push(std::make_unique<task<T>>(std::forward<T>(msg), logger->getDefaultLogLevel()));
            }
            cv_.notify_one();
        }

        ~worker();

        void stop();

    private:
        struct virtual_task {
            virtual ~virtual_task() = default;
            virtual std::string get_msg() = 0;
            virtual level::logLevel get_ll() = 0;
        };
        template <typename T>
        struct task : virtual_task {
            task() {}
            task(T&& m, level::logLevel l) : msg(m), ll(l) {}
            std::string get_msg() override {
                std::ostringstream os;
                os << msg;
                return os.str();
            }
            level::logLevel get_ll() override {
                return ll;
            }
            T msg;
            level::logLevel ll;
        };
        
        std::unique_ptr<logWriter::logger> logger;
        std::queue<std::unique_ptr<virtual_task>> q;

        std::mutex m_;
        std::condition_variable cv_;
        std::vector<std::thread> threads_;
        std::atomic<bool> running_;

        void worker_loop();

        void handle_task(std::unique_ptr<virtual_task>& t);
    };
}

std::string get_message();

std::string get_loglevel();

int run_app(int argc, char *argv[]);

}