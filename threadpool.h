#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();
        template <typename Callable, typename... Args>
        auto submit(Callable&& func, Args&&... args) -> std::future<std::result_of_t<Callable(Args...)>>;
        void stopWorkers();
        
    private:
        int num_threads = std::max(1u, std::thread::hardware_concurrency() / 2);
        template <typename Callable, typename... Args>
        std::queue<std::pair<std::function<std::result_of_t<Callable(Args...)>(Args...)>, std::promise<std::result_of_t<Callable(Args...)>>>> jobQueue;
        std::vector<std::thread> workers;
        std::mutex jobQueueMutex;
        std::condition_variable cond;
        std::atomic_bool stopFlag{false};
        void deployWorker();
};

ThreadPool::ThreadPool() {
  // Launch threads in parallel
  for (int i = 0; i < num_threads; ++i) {
      workers.emplace_back(&ThreadPool::deployWorker, std::ref(*this));
  }
}

ThreadPool::~ThreadPool() {
  // Wait for all threads to finish
  for (auto& worker : workers) {
      if (worker.joinable()) {
          worker.join();
      }
  }
}

template <typename Callable, typename... Args>
auto ThreadPool::submit(Callable&& func, Args&&... args) -> std::future<std::result_of_t<Callable(Args...)>> {
  // Create future for return value
  std::promise<std::result_of_t<Callable(Args...)>> p;
  std::future<std::result_of_t<Callable(Args...)>> f = p.get_future();

   std::function<std::result_of_t<Callable(Args...)>(Args...)> funct(func, args...);

  // Add function to queue and wake up thread
  jobQueue.push({funct, std::move(p)});
  cond.notify_one();

  return f;
}

// Starts worker thread which infintely loops watching the queue for jobs
void ThreadPool::deployWorker() {
  while(!stopFlag.load()) {
    // Use unique_lock to lock the mutex
    std::unique_lock<std::mutex> lock(jobQueueMutex);

    // Will unlock mutex and sleep until notify_one() signal (function enters jobQueue), 
    // then wake up, lock the mutex and continue on
    cond.wait(lock, [this](){ return (!jobQueue.empty() || stopFlag.load()); });

    if (!stopFlag.load()) {
      // Pop front function in queue
      auto funcPtr = jobQueue.front().first;
      auto prom = std::move(jobQueue.front().second);
      jobQueue.pop();

      // Unlock mutex, and wake up one thread
      lock.unlock();
      
      // Run the function
      (*funcPtr)();
      int res = 0; // ! find a way to set the return value of function to this var

      // Set promise to return value
      prom.set_value(res);
    }
  }
}

// Stops workers by setting stopFlag = 1
void ThreadPool::stopWorkers() { 
  // Set flag to stop workers from looping
  stopFlag.store(true);

  // Wake up workers
  cond.notify_all(); 

  // Wait for all threads to finish
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

#endif