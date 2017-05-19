#include "ksr/functional.hpp"
#include "ksr/std/array.hpp"

#include "catch/catch.hpp"
#include <algorithm>
#include <array>
#include <iterator>

using namespace ksr;
namespace {

    struct agg {
        int value;
    };

    bool operator==(const agg lhs, const agg rhs) {
        return lhs.value == rhs.value;
    }

    constexpr auto arr = make_array(agg{0}, agg{1}, agg{2});
}

TEST_CASE("mem_equals", "[functional][predicates]") {
    CHECK(std::any_of(std::cbegin(arr), std::cend(arr), mem_equals<&agg::value>{2}));
    CHECK(std::none_of(std::cbegin(arr), std::cend(arr), mem_equals<&agg::value>{-1}));
}

TEST_CASE("mem_less", "[functional][predicates]") {
    auto temp = make_array(agg{2}, agg{1}, agg{0});
    std::sort(std::begin(temp), std::end(temp), mem_less<&agg::value>{});
    CHECK(temp == arr);
}
