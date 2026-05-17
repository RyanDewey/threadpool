#include "threadpool.h"
#include <iostream>
#include <memory>

void sayHi() {
    std::cout << "Hi" << std::endl;
}

int main() {
    
    std::shared_ptr<void(*)()> funcPtr = std::make_shared<void(*)()>(sayHi);

    ThreadPool pool;
    pool.submit(funcPtr);
    pool.submit(funcPtr);
    pool.submit(funcPtr);

    pool.runFunc();
    pool.runFunc();
    pool.runFunc();


    std::cout << "Main ran successfully!" << std::endl;
    return 0;
}