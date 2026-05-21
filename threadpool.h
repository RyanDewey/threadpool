#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();
        void submit(std::shared_ptr<void(*)()>);
        

    private:
        int num_threads = std::thread::hardware_concurrency() - 1;
        std::queue<std::shared_ptr<void(*)()>> jobQueue;
        std::vector<std::thread> workers;
        std::mutex jobQueueMutex;
        std::condition_variable cond;
        void deployWorker();
};

#endif