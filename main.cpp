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

struct MultiplyBy {
    int factor;

    MultiplyBy(int f) : factor(f) {}

    int operator()(int n) const {
        return n * factor;
    }
};

class Num {
    public:
        int makeInt(double x) { return int(x); }
};

int main() {

    ThreadPool pool;

    MultiplyBy triple(3);
    Num myNum;
    
    pool.submit(sayHi);
    auto fut1 = pool.submit(retThree);
    auto fut2 = pool.submit(retThreePointThree);
    auto fut3 = pool.submit(add, 6, 7);
    auto fut4 = pool.submit([]() { std::cout << "Hello World" << std::endl; });
    auto fut5 = pool.submit(triple, 4);
    auto fut6 = pool.submit([&myNum](){ return myNum.makeInt(3.3); });


    std::cout << "Future 1: " << fut1.get() << std::endl
              << "Future 2: " << fut2.get() << std::endl
              << "Future 3: " << fut3.get() << std::endl
              << "Future 5: " << fut5.get() << std::endl
              << "Future 6: " << fut6.get() << std::endl
              << std::endl;
    
    std::cout << std::endl << "Main ran successfully!" << std::endl;
    return 0;
}