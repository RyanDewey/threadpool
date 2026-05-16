cmake_minimum_required(VERSION 3.16)
project(threadpool CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)
add_executable(threadpool 
    main.cpp
    threadpool.cpp)
find_package(Threads REQUIRED)
target_link_libraries(threadpool PRIVATE Threads::Threads)