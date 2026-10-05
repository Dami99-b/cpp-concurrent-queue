#include "bounded_queue.hpp"

#include <cassert>
#include <iostream>
#include <thread>

void test_fifo_order() {
    std::cout << "Creating queue..." << std::endl;
    BoundedQueue<int> queue(2);

    std::cout << "Pushing 10..." << std::endl;
    bool pushed10 = queue.push(10);
    assert(pushed10);

    std::cout << "Pushing 20..." << std::endl;
    bool pushed20 = queue.push(20);
    assert(pushed20);

    std::cout << "Popping first..." << std::endl;
    auto first = queue.pop();

    std::cout << "Popping second..." << std::endl;
    auto second = queue.pop();

    assert(first.has_value() && *first == 10);
    assert(second.has_value() && *second == 20);

    std::cout << "Closing queue..." << std::endl;
    queue.close();

    std::cout << "Popping after close..." << std::endl;
    assert(!queue.pop().has_value());

    std::cout << "FIFO test passed." << std::endl;
}

void test_close_unblocks_queue() {
    BoundedQueue<int> queue(1);
    queue.close();

    assert(!queue.push(42));
    assert(!queue.pop().has_value());
}

void test_concurrent_producers_and_consumers() {
    BoundedQueue<int> queue(64);

    constexpr int producer_count = 4;
    constexpr int items_per_producer = 1'000;

    std::atomic<int> consumed{0};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < producer_count; ++i) {
        producers.emplace_back([&queue] {
            for (int j = 0; j < items_per_producer; ++j) {
                bool pushed = queue.push(j);
                assert(pushed);
            }
        });
    }

    for (int i = 0; i < 4; ++i) {
        consumers.emplace_back([&queue, &consumed] {
            while (true) {
                auto value = queue.pop();

                if (!value.has_value()) {
                    break;
                }

                ++consumed;
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }

    queue.close();

    for (auto& consumer : consumers) {
        consumer.join();
    }

    assert(consumed == producer_count * items_per_producer);
}

int main() {
    std::cout << "Running FIFO test..." << std::endl;
    test_fifo_order();
    std::cout << "Running close test..." << std::endl;
    test_close_unblocks_queue();
    std::cout << "Running concurrency test..." << std::endl;
    test_concurrent_producers_and_consumers();

    std::cout << "All tests passed.\n";
    return 0;
}
