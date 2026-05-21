#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

class ThreadPool {
    public:
        ThreadPool() = default;
        void submit(std::shared_ptr<void(*)()>);
        void spinUpWorkers();
        

    private:
        int num_threads = std::thread::hardware_concurrency() - 1;
        std::queue<std::shared_ptr<void(*)()>> jobQueue;
        std::mutex jobQueueMutex;
        std::condition_variable cond;
        void deployWorker();
};

#endif