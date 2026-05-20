#include "threadpool.h"
#include <iostream> // ! REMOVE WHEN DONE TESTING

void ThreadPool::submit(std::shared_ptr<void (*)()> funcPtr) {
  jobQueue.push(funcPtr);
}

// Testing function
void ThreadPool::runFuncs() {

  // Run all functions in jobQueue
  while (!jobQueue.empty()) {
    // Pop front function in queue
    std::shared_ptr<void (*)()> funcPtr = jobQueue.front();
    jobQueue.pop();

    // Run the function
    (*funcPtr)();
  }
}

// Starts worker thread which infintely loops watching the queue for jobs
void ThreadPool::deployWorker() {
  while(true) {
    if (!jobQueue.empty()) {
      // ! probably lock the queue using mutex to avoid race conditions
      // Pop front function in queue
      std::shared_ptr<void (*)()> funcPtr = jobQueue.front();
      jobQueue.pop();
      // ! unlock mutex
      std::cout << "worker running function from jobQueue" << std::endl;
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
  // While available threads -> start them as workers
  std::thread worker(&ThreadPool::deployWorker, *this);
  worker.join();
  
}