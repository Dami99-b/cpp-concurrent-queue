#include "bounded_queue.hpp"

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    constexpr int producer_count = 4;
    constexpr int consumer_count = 4;
    constexpr int items_per_producer = 25'000;

    BoundedQueue<int> queue(256);
    std::atomic<int> consumed{0};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < producer_count; ++i) {
        producers.emplace_back([&queue, i] {
            for (int j = 0; j < items_per_producer; ++j) {
                queue.push(i * items_per_producer + j);
            }
        });
    }

    for (int i = 0; i < consumer_count; ++i) {
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

    std::cout << "Consumed items: " << consumed.load() << '\n';
    std::cout << "Expected items: "
              << producer_count * items_per_producer << '\n';

    return 0;
}
