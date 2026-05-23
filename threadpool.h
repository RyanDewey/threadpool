#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <csignal>
#include <type_traits>

class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();
        void submit(std::shared_ptr<void(*)()>);
        void stopWorkers();
        
    private:
        int num_threads = std::max(1u, std::thread::hardware_concurrency() / 2);
        std::queue<std::shared_ptr<void(*)()>> jobQueue;
        std::vector<std::thread> workers;
        std::mutex jobQueueMutex;
        std::condition_variable cond;
        std::atomic_bool stopFlag{false};
        void deployWorker();
        
};

#endif