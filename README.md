# Threadpool
**C++ implementation of a ThreadPool class that lets users easily spin up threads that persist and execute tasks added to a queue.** 

### How to use
```cpp
#include "ThreadPool.h"
#include <memory>

int main() {
    // Make shared_ptr to function
    std::shared_ptr<void(*)()> funcPtr = std::make_shared<void(*)()>(funcName);

    ThreadPool pool; // Init a threadpool obj

    pool.submit(funcPtr); // Add function to queue

    std::cout << "ThreadPool will automatically complete all tasks added to queue concurrently" << std::endl;
    return 0;
}
```