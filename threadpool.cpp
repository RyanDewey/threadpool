#include "threadpool.h"

void ThreadPool::submit(std::shared_ptr<void(*)()> funcPtr) {
    jobQueue.push(funcPtr);
}

void ThreadPool::runFunc() {
    // Pop front function in queue
    std::shared_ptr<void(*)()> funcPtr = jobQueue.front();
    jobQueue.pop();

    // Run the function
    (*funcPtr)();
    
}