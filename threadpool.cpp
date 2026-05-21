#include "threadpool.h"
#include <thread>
#include <vector>
#include <chrono>

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

void ThreadPool::submit(std::shared_ptr<void (*)()> funcPtr) {
  jobQueue.push(funcPtr);
  cond.notify_one();
}

// Starts worker thread which infintely loops watching the queue for jobs
void ThreadPool::deployWorker() {
  while(true) {
    // Use unique_lock to lock the mutex
    std::unique_lock<std::mutex> lock(jobQueueMutex);

    // Will unlock mutex and sleep until notify_one() signal (function enters jobQueue), 
    // then wake up, lock the mutex and continue on
    cond.wait(lock, [this](){ return !jobQueue.empty(); });

    // Pop front function in queue
    std::shared_ptr<void (*)()> funcPtr = jobQueue.front();
    jobQueue.pop();

    // Unlock mutex, and wake up one thread
    lock.unlock();
    
    // Run the function
    (*funcPtr)();
    
  }
}