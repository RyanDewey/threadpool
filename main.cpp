#include "threadpool.h"
#include <iostream>
#include <memory>

void sayHi() {
    std::cout << "Hi" << std::endl;
}

void sayHello() {
    std::cout << "Hello" << std::endl;
}

int main() {
    
    std::shared_ptr<void(*)()> hiPtr = std::make_shared<void(*)()>(sayHi);
    std::shared_ptr<void(*)()> helloPtr = std::make_shared<void(*)()>(sayHello);


    ThreadPool pool;
    
    pool.submit(hiPtr);
    pool.submit(helloPtr);
    pool.submit(hiPtr);


    pool.spinUpWorkers();
    

    std::cout << "Main ran successfully!" << std::endl;
    return 0;
}