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

```
## Requirements

- C++17-compatible compiler
- CMake 3.16 or later
- Git
- POSIX-compatible shell for the example commands

The project was developed and tested in Termux using Clang.

```

```
Build the project:

```bash
cmake --build build-debug
```

The build creates two executables:

```text
build-debug/queue_demo
build-debug/queue_tests
```

## Run the Tests

Run the tests directly:

```bash
./build-debug/queue_tests
```

Run them through CTest:

```bash
ctest --test-dir build-debug --output-on-failure
```

Expected output:

```text
100% tests passed, 0 tests failed out of 1
```

The test suite covers:

- FIFO ordering
- Rejecting pushes after shutdown
- Returning an empty result after the queue is closed and drained
- Multiple concurrent producers
- Multiple concurrent consumers
- Correct delivery of all produced items

## Run the Demo

```bash
./build-debug/queue_demo
```

Expected output:

```text
Consumed items: 100000
Expected items: 100000
```

The demo starts four producer threads and four consumer threads.
Each producer creates 25,000 integers, giving a total of 100,000
items. After all producers finish, the queue is closed and the
consumers drain the remaining items before terminating.

## Design

The queue is implemented as a class template:

```cpp
template <typename T>
class BoundedQueue;
```

This allows it to store different value types, such as:

```cpp
BoundedQueue<int> numbers(64);
BoundedQueue<std::string> messages(64);
```

The queue uses the following synchronization primitives:

- `std::mutex` protects the queue and shutdown state.
- `std::condition_variable not_full_` coordinates producers.
- `std::condition_variable not_empty_` coordinates consumers.
- `std::queue<T>` provides FIFO storage.
- `closed_` records whether shutdown has started.

### `push()`

A producer waits until either:

1. The queue has available capacity, or
2. The queue has been closed.

If the queue is closed, `push()` returns `false`. Otherwise, it moves
the value into the queue and wakes one waiting consumer.

### `pop()`

A consumer waits until either:

1. The queue contains an item, or
2. The queue has been closed.

If the queue is closed and empty, `pop()` returns `std::nullopt`.
Otherwise, it removes and returns the oldest item.

### `close()`

Calling `close()`:

1. Marks the queue as closed.
2. Wakes all blocked producers.
3. Wakes all blocked consumers.
4. Prevents future calls to `push()` from adding items.

Closing does not immediately discard items already in the queue.
Consumers can continue removing existing items until the queue is empty.

## API

### Constructor

```cpp
explicit BoundedQueue(std::size_t capacity);
```

Creates a queue with a fixed maximum capacity.

The capacity must be greater than zero.

### Push

```cpp
bool push(T value);
```

Blocks while the queue is full.

Returns:

- `true` if the item was added
- `false` if the queue was already closed

### Pop

```cpp
std::optional<T> pop();
```

Blocks while the queue is empty.

Returns:

- The next item if one is available
- `std::nullopt` if the queue is closed and empty

### Close

```cpp
void close();
```

Closes the queue and wakes all waiting threads.

### Status Methods

```cpp
bool closed() const;
std::size_t size() const;
```

These methods safely report the queue's current state.

## Complexity

Ignoring thread scheduling and lock contention:

- `push()`: O(1)
- `pop()`: O(1)
- `close()`: O(1)
- `size()`: O(1)
- Memory usage: O(capacity)

## Concurrency and Shutdown

The queue uses condition-variable predicates rather than relying
only on notifications. This protects against spurious wakeups.

For example, consumers wait using a condition equivalent to:

```cpp
!queue_.empty() || closed_
```

This ensures that a consumer only proceeds when there is an item to
consume or when shutdown means it should terminate.

The demo follows this shutdown order:

1. Start producers and consumers.
2. Wait for all producers to finish.
3. Call `queue.close()`.
4. Wait for all consumers to finish.

This ensures that no producer is still trying to add items when the
queue is closed.

## Testing Notes

The tests are compiled in Debug mode so that assertions are active.

Function calls with side effects should not be placed directly inside
`assert()` expressions when assertions may be disabled. For example,
this is unsafe in a Release build:

```cpp
assert(queue.push(42));
```

The safer pattern is:

```cpp
bool pushed = queue.push(42);
assert(pushed);
```

The queue operation executes regardless of whether assertions are
enabled.

## Limitations

This is an educational mutex-based queue implementation. It is not
intended to replace specialized lock-free queues in latency-critical
systems.

Current limitations include:

- No timed `push()` or `pop()` operations
- No non-blocking `try_push()` or `try_pop()` operations
- No queue reset or reopen operation after `close()`
- No performance benchmark suite
- No formal memory-ordering or lock-free implementation
- No configurable test workloads

## Possible Improvements

Future versions could add:

- `try_push()` and `try_pop()`
- `push_for()` and `pop_for()` timeout operations
- Move-only and non-copyable value tests
- Better test assertions that remain active in Release builds
- ThreadSanitizer testing
- Performance benchmarks
- A comparison with a lock-free queue
- GitHub Actions CI
- Documentation generated with Doxygen
- Support for cancellation using `std::stop_token`
- Separate producer-consumer benchmark configurations

## Example Usage

```cpp
#include "bounded_queue.hpp"

#include <iostream>

int main() {
    BoundedQueue<int> queue(สอง);

    queue.push(10);
    queue.push(20);

    auto first = queue.pop();

    if (first.has_value()) {
        std::cout << *first << '\n';
    }

    queue.close();
}
```

Replace `สอง` with `2` if you copy this example. The intended
constructor call is:

```cpp
BoundedQueue<int> queue(2);
```

## License

MIT License

Copyright (c) 2026

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files, to deal in
the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
