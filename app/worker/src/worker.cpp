#include "../include/worker.hpp"
		
namespace app {

namespace logWorker {

worker::worker(const std::string& filename, const std::string& default_loglevel, size_t number_of_threads) 
: logger(std::make_unique<logWriter::logger>(filename, level::str_to_loglevel(default_loglevel))), running_(true)
{
    threads_.reserve(number_of_threads);
    for(size_t i = 0; i < number_of_threads; i++) {
        threads_.emplace_back(&worker::worker_loop, this);
    }
}

worker::~worker() {
    stop();
    for(auto& th_ : threads_) {
        th_.join();
    }
    logger->finish();
}

void worker::stop() {
    if(!running_) return;
    running_.exchange(false);
    cv_.notify_all();
}

void worker::worker_loop() {
    std::string msg;
    std::string loglevel;
    while(running_ || !q.empty()) {
        std::unique_ptr<virtual_task> t;
        {
            std::unique_lock<std::mutex> lock(m_);
            cv_.wait(lock, [this](){
                return !running_ || !q.empty();
            });
            if(!running_ && q.empty()) {
                return;
            }
            if (q.empty()) continue;
            t = std::move(q.front());
            q.pop();
        }
        handle_task(t);
    }
}

void worker::handle_task(std::unique_ptr<virtual_task>& t) {
    logger->log(t->get_msg(), t->get_ll());
}

}

std::string get_message(){
    std::string msg;
    std::cout << "Enter Log Message: " << "\n";
    std::getline(std::cin, msg);
    return msg;
}

std::string get_loglevel(){
    std::string loglevel;
    std::cout << "Enter Log Level: " << "\n";
    std::getline(std::cin, loglevel);
    return loglevel;
}

int run_app(int argc, char *argv[]){
    if(argc < 2) {
        std::cerr << "Usage: .\\logger <filename> <LOGLEVEL: DEBUG/INFO/CRITICAL>?\n";
        return 1;
    }

    std::string ll_str;
    ll_str = (argc == 2) ? "INFO" : argv[2];
    logWorker::worker worker(argv[1], ll_str);
    while(true) {
        std::string msg = get_message();
        if(msg == "exit") {
            worker.stop();
            break;
        }
        std::string ll = get_loglevel();
        worker.log(std::move(msg), ll);
    }

    return 0;
}

}