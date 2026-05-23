#include "threadpool.h"
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include <chrono>

ThreadPool::ThreadPool() {
  // Launch threads in parallel
  for (int i = 0; i < num_threads; ++i) {
      workers.emplace_back(&ThreadPool::deployWorker, std::ref(*this));
  }
}

ThreadPool::~ThreadPool() {
  // Wait for return values
  for (auto& f : futures) {
    f.wait();     
  }

  // Wait for all threads to finish
  for (auto& worker : workers) {
      if (worker.joinable()) {
          worker.join();
      }
  }
}

std::future<int> ThreadPool::submit(std::shared_ptr<void (*)()> funcPtr) {
  // Create future for return value
  std::promise<int> p;
  std::future<int> f = p.get_future();
  futures.emplace_back(f);

  jobQueue.push({funcPtr, std::move(p)});
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
      auto p = std::move(jobQueue.front().second);
      jobQueue.pop();

      // Unlock mutex, and wake up one thread
      lock.unlock();
      
      // Run the function
      (*funcPtr)();
      int res = 0; // ! find a way to set the return value of function to this var

      // Set promise to return value
      p.set_value(res);
    }
  }
}

// Stops workers by setting stopFlag = 1
void ThreadPool::stopWorkers() { 
  // Set flag to stop workers from looping
  stopFlag.store(true);

  // Wake up workers
  cond.notify_all(); 

  // Wait for return values
  for (auto& f : futures) {
    f.wait();     
  }

  // Wait for all threads to finish
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}