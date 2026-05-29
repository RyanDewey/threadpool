#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

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
    ThreadPool() { // Launch threads in parallel
      for (int i = 0; i < num_threads; ++i) {
          workers.emplace_back(&ThreadPool::deployWorker, std::ref(*this));
      }
    }

    ~ThreadPool() { // Wait for all threads to finish
      for (auto& worker : workers) {
        if (worker.joinable()) worker.join();
      }
    }

    template <typename Callable, typename... Args>
    auto submit(Callable&& func, Args&&... args) -> std::future<std::result_of_t<Callable(Args...)>> {
      typedef std::result_of_t<Callable(Args...)> retType;

      // Bind arguments to function, and make the callable a packaged_task
      std::packaged_task<retType()> task(std::bind(func, std::forward<Args>(args)...));

      // Get future to return to caller
      std::future<retType> result = task.get_future();
      
      // Add function to queue and wake up thread
      jobQueue.push(std::move(task));
      cond.notify_one();

      return result;
    }

    void stopWorkers() {
      stopFlag.store(true); // Set flag to stop workers from looping

      cond.notify_all(); // Wake up workers

      // Wait for all threads to finish
      for (auto& worker : workers) {
          if (worker.joinable()) {
              worker.join();
          }
      }
    }
        
  private:
    int num_threads = std::max(1u, std::thread::hardware_concurrency() / 2);
    std::queue<AnyCallable> jobQueue;
    std::vector<std::thread> workers;
    std::mutex jobQueueMutex;
    std::condition_variable cond;
    std::atomic_bool stopFlag{false};

    // Starts worker thread which infintely loops watching the queue for jobs
    void deployWorker() { 
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
};

#endif