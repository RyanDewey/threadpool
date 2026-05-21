#ifndef THREADPOOL_H_
#define THREADPOOL_H_

// Class that has a queue of jobs and a pool of workers that complete the jobs concurrently.
// instantiate an object of the threadpool class, which should have methods to push jobs 
// (functions) to the queue. The thread pool will be spun up at time of construction (or when 
// the first functions take over the threads) and then threads will die when all functions done
// Return values to the functions should be returned via "futures"

#include <queue>
#include <thread>
#include <memory>

class ThreadPool {
    public:
        ThreadPool() = default;
        void submit(std::shared_ptr<void(*)()>);
        void spinUpWorkers();
        

    private:
        int num_threads = std::thread::hardware_concurrency() - 1;
        std::queue<std::shared_ptr<void(*)()>> jobQueue;
        void deployWorker();
};

#endif