#include "threadpool.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include <cmath>

void sayHi() { std::cout << "Hi" << std::endl; }

int main() {
    
    std::shared_ptr<void(*)()> hiPtr = std::make_shared<void(*)()>(sayHi);

    ThreadPool pool;
    
    
    pool.submit(hiPtr);
    

    std::cout << std::endl << "Main ran successfully!" << std::endl;
    return 0;
}