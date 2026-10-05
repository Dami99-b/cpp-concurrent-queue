# C++ Concurrent Bounded Queue

A thread-safe, bounded FIFO queue implemented in C++17 using
`std::mutex` and `std::condition_variable`.

The project demonstrates a producer-consumer system in which producers
block when the queue is full, consumers block when the queue is empty,
and the queue supports graceful shutdown through an explicit `close()`
operation.

## Features

- Thread-safe FIFO queue
- Bounded capacity
- Multiple concurrent producers
- Multiple concurrent consumers
- Blocking `push()` when the queue is full
- Blocking `pop()` when the queue is empty
- Graceful shutdown using `close()`
- C++17 implementation
- CMake build system
- Automated tests using CTest
- Demonstration workload processing 100,000 items

## Project Structure

```text
.
├── CMakeLists.txt
├── README.md
├── include/
│   └── bounded\_queue.hpp
├── src/
│   └── main.cpp
├── tests/
│   └── test\_queue.cpp
└── build/
