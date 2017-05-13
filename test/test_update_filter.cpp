#include "ksr/update_filter.hpp"

#include "catch/catch.hpp"

#include <chrono>
#include <thread>

using namespace ksr;
using namespace std::literals::chrono_literals;

TEST_CASE("int_percentage_count", "[update_filter]") {

    auto update_count = 0;
    auto filter = int_percentage_filter<int>{[&](int, int) {
        ++update_count;
    }};

    static constexpr auto max = 1000;
    for (auto i = 0; i < max; ++i) {
        filter.update(i, max);
    }

    CHECK(update_count == 101);
    CHECK(int_percentage(filter) == 100);

    filter.sync(max, max);
    CHECK(update_count == 102);
    CHECK(int_percentage(filter) == 100);
}

TEST_CASE("double_percentage_count", "[update_filter]") {

    auto update_count = 0;
    auto filter = int_percentage_filter<double>{[&](double, double) {
        ++update_count;
    }};

    static constexpr auto max = 1.0;
    for (auto i = 0.0; i < max; i += 0.001) {
        filter.update(i, max);
    }

    CHECK(update_count == 101);
    CHECK(int_percentage(filter) == 100);

    filter.sync(max, max);
    CHECK(update_count == 102);
    CHECK(int_percentage(filter) == 100);
}

TEST_CASE("sample_count", "[update_filter]") {

    auto update_count = 0;
    auto filter = sampled_filter<int>{20ms, [&](int) {
        ++update_count;
    }};

    auto i = 0;
    const auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < 50ms) {
        filter.update(i++);
    }

    CHECK(update_count == 3);

    filter.sync(i);
    CHECK(update_count == 4);
}

TEST_CASE("sample_period", "[update_filter]") {

    static constexpr auto final_value = 10;
    static constexpr auto update_period = 5ms;
    static constexpr auto sample_period = 20ms;

    auto update_count = 0;
    auto value = 0;
    auto filter = sampled_filter<int>{sample_period, [&](const int new_value) {
        ++update_count;
        value = new_value;
    }};

    for (int i = 0; i < final_value; ++i) {
        filter.update(i);
        std::this_thread::sleep_for(update_period);
    }

    static constexpr auto max_updates = final_value * update_period / sample_period + 1;
    CHECK(update_count <= max_updates);
    CHECK(value == final_value - 1);

    filter.sync(final_value);
    CHECK(value == final_value);
}
