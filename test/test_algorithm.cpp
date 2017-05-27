#include "ksr/algorithm.hpp"

#include "catch/catch.hpp"
#include <iterator>
#include <utility>
#include <vector>

using namespace ksr;
using seq = std::vector<int>;
using meta_seq = std::vector<seq>;

namespace {

    auto has_sub_permutations(seq domain, const meta_seq& expected) -> bool {

        auto actual = meta_seq{};
        const auto push_back = [&actual](seq sub_permutation) {
            actual.push_back(std::move(sub_permutation));
        };

        sub_permute(std::begin(domain), std::end(domain), push_back);
        return actual == expected;
    }
}

TEST_CASE("sub_permute", "[algorithm]") {

    CHECK(has_sub_permutations({}, {{}}));
    CHECK(has_sub_permutations({0}, {{}, {0}}));
    CHECK(has_sub_permutations({0, 1}, {{}, {0}, {1}, {0, 1}, {1, 0}}));

    {
        auto domain = seq{0, 1, 2};
        const auto expected = meta_seq{
            {}, {0}, {1}, {2},
            {0, 1}, {0, 2}, {1, 0}, {1, 2}, {2, 0}, {2, 1},
            {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};

        CHECK(has_sub_permutations(std::move(domain), expected));
    }
}
