#include "threadpool.h"

void ThreadPool::submit(std::shared_ptr<void()> funcPtr) {
    jobQueue.push(funcPtr);
}