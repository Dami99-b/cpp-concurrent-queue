cmake_minimum_required(VERSION 3.16)
project(cpp_concurrent_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(concurrent_queue INTERFACE)
target_include_directories(concurrent_queue INTERFACE include)

add_executable(queue_demo src/main.cpp)
target_link_libraries(queue_demo PRIVATE concurrent_queue)

enable_testing()

add_executable(queue_tests tests/test_queue.cpp)
target_link_libraries(queue_tests PRIVATE concurrent_queue)

add_test(NAME queue_tests COMMAND queue_tests)
