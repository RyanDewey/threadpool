#include "threadpool.h"
#include <chrono>
#include <iostream>
#include <memory>

int sayHi() { 
    std::cout << "Hi" << std::endl;
    return 2;
 }

int main() {

    ThreadPool pool;
    
    auto fut1 = pool.submit(sayHi);
    auto fut2 = pool.submit([]() {std::cout << "Hello World" << std::endl; });
    
    std::cout << std::endl << "Main ran successfully!" << std::endl;


    std::chrono::seconds(5);

    std::cout << fut1.get() << std::endl;
    return 0;
}