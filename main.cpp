#include "threadpool.h"

#include <iostream>
#include <memory>

void sayHi() { std::cout << "Hi" << std::endl; }

int main() {
    
    std::shared_ptr<void(*)()> hiPtr = std::make_shared<void(*)()>(sayHi);

    ThreadPool<int> pool;
    
    auto fut = pool.submit();
    

    std::cout << std::endl << "Main ran successfully!" << std::endl;
    return 0;
}