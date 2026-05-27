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

// TYPE ERASURE CLASSES

// Abstract Base class
class CallableBase {
  public:
    virtual ~CallableBase() = default;
    virtual void call() = 0;
};

// Derived implementation class
template <typename T>
class CallableImpl : public CallableBase {
    T callable;
  public:
    explicit CallableImpl(T x) : callable(std::move(x)) {}
    void call() override { callable(); }

};

// Wrapper class
class AnyCallable {
    std::unique_ptr<CallableBase> self_;
  public:
    template <typename T>
    AnyCallable(T x) : self_(std::make_unique<CallableImpl<T>>(std::move(x))) {}
    void call() { self_->call(); }
};

// THREAD POOL CLASS

class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();
        template <typename Callable, typename... Args>
        auto submit(Callable&& func, Args&&... args) -> std::future<std::result_of_t<Callable(Args...)>>;
        void stopWorkers();
        
    private:
        int num_threads = std::max(1u, std::thread::hardware_concurrency() / 2);
        std::queue<AnyCallable> jobQueue;
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

  typedef std::result_of_t<Callable(Args...)> retType;

  // Make the callable a packaged_task
  std::packaged_task<retType(Args...)> task(func, args...);
  auto f = task.get_future();
   
  // Add function to queue and wake up thread
  jobQueue.push(std::move(task));
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
      auto callable = std::move(jobQueue.front());
      jobQueue.pop();

      // Unlock mutex, and wake up one thread
      lock.unlock();
      
      // Run the function
      callable.call();
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