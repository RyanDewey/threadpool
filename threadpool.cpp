#include "threadpool.h"
#include <vector>

void ThreadPool::submit(std::shared_ptr<void (*)()> funcPtr) {
  jobQueue.push(funcPtr);
}

// Starts worker thread which infintely loops watching the queue for jobs
void ThreadPool::deployWorker() {
  while(true) {
    if (!jobQueue.empty()) {
      // Use lock guard to lock the mutex, it unlocks when it goes out of scope
      std::lock_guard<std::mutex> lock(jobQueueMutex);
      // Pop front function in queue
      std::shared_ptr<void (*)()> funcPtr = jobQueue.front();
      jobQueue.pop();
      
      // Run the function
      (*funcPtr)();
    } else {
      // sleep using wait, and wake it by sending signal when task enters the Q
      //std::cout << "worker sleeping" << std::endl;
    }
  }
}

// Creates all available workers and deploys them
void ThreadPool::spinUpWorkers() { 
  
  // Create a vector to hold the threads
  std::vector<std::thread> workers;

  // Launch threads in parallel
  for (int i = 0; i < num_threads; ++i) {
      workers.emplace_back(&ThreadPool::deployWorker, std::ref(*this));
  }

  // Wait for all threads to finish
  for (auto& worker : workers) {
      if (worker.joinable()) {
          worker.join();
      }
  }
  
}