#include "threadpool.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include <cmath>

void heavyComputeTask() {
    volatile double result = 0.0; 
    
    for (int i = 0; i < 1000; ++i) {
        // Simple math that keeps the CPU busy without frequent memory access
        result += std::sin(i) * std::cos(i);
    }
}

int main() {
    
    
    std::shared_ptr<void(*)()> heavyTaskPtr = std::make_shared<void(*)()>(heavyComputeTask);

    // // Capture start time
    // auto syncStart = std::chrono::high_resolution_clock::now();

    // // Synchronus test
    // for (int i = 0; i < 1000; ++i) {
    //     heavyComputeTask();
    // }

    // // Capture end time
    // auto syncEnd = std::chrono::high_resolution_clock::now();

    // // Calculate elapsed time in milliseconds
    // auto syncDuration = std::chrono::duration_cast<std::chrono::milliseconds>(syncEnd - syncStart).count();
    // std::cout << "Duration of synchronus test: " << syncDuration << " milliseconds" << std::endl;


    ThreadPool pool;

    for (int i = 0; i < 1000; ++i) {
        pool.submit(heavyTaskPtr);
    }


    pool.stopWorkers();


    // // Capture start time
    // auto asyncStart = std::chrono::high_resolution_clock::now();
    
    // for (int i = 0; i < 1000; ++i) {
    //     pool.submit(heavyTaskPtr);
    // }
    
    
    // // Capture end time
    // auto asyncEnd = std::chrono::high_resolution_clock::now();

    // // Calculate elapsed time in milliseconds
    // auto asyncDuration = std::chrono::duration_cast<std::chrono::milliseconds>(asyncEnd - asyncStart).count();
    // std::cout << "Duration of asynchronus test: " << asyncDuration << " milliseconds" << std::endl;

    std::cout << std::endl << "Main ran successfully!" << std::endl;
    return 0;
}