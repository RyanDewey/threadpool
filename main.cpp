#include "threadpool.h"

#include <chrono>
#include <iostream>
#include <thread>

void sayHi() { 
    std::cout << "Hi" << std::endl;
}

int retThree() {
    return 3;
}

double retThreePointThree() {
    return 3.3;
}

int add(int a, int b) {
    return a + b;
}

int main() {

    ThreadPool pool;
    
    pool.submit(sayHi);
    auto fut1 = pool.submit(retThree);
    auto fut2 = pool.submit(retThreePointThree);
    auto fut3 = pool.submit(add, 6, 7);
    auto fut4 = pool.submit([]() {std::cout << "Hello World" << std::endl; });

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << fut3.get() << std::endl;
    
    std::cout << std::endl << "Main ran successfully!" << std::endl;
    return 0;
}