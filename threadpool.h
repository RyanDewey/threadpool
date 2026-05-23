#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <csignal>
#include <future>

class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();
        std::future<int> submit(std::shared_ptr<void(*)()>);
        void stopWorkers();
        
    private:
        int num_threads = std::max(1u, std::thread::hardware_concurrency() / 2);
        std::queue<std::pair<std::shared_ptr<void(*)()>, std::promise<int>>> jobQueue;
        std::vector<std::thread> workers;
        std::vector<std::promise<int>> promises;
        std::vector<std::future<int>> futures;
        std::mutex jobQueueMutex;
        std::condition_variable cond;
        std::atomic_bool stopFlag{false};
        void deployWorker();
};

#endif