#include "ksr/algorithm.hpp"
#include "ksr/flag_product.hpp"

#include "catch/catch.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <iterator>
#include <numeric>

using namespace ksr;
namespace {

    enum class config_1 { opt_1, opt_2 };
    enum class config_2 { opt_1, opt_2 };

    enum class flags : std::uint8_t {
        flag_1 = (1 << 0),
        flag_2 = (1 << 1),
        flag_3 = (1 << 2)
    };

    KSR_DEFINE_FLAGS(flags)
    auto flag_domain = std::array{flags::flag_1, flags::flag_2, flags::flag_3};
    using flag_domain_iter = decltype(flag_domain)::iterator;

    /// Given a range of `flags` codes describing a particular permutation of options, verifies that
    /// combining those flags using various bitwise-or operations gives a compound value that can be
    /// subsequently decomposed into each individual input flags (and only those input flags). More
    /// formally, checks that the `|` and `|=` operators for `flags` behave in a consistent and
    /// commutative way, and that the `&` operator provides the inverse operation.

    void check_permutation(const flag_domain_iter begin, const flag_domain_iter end) {

        const auto rbegin = std::reverse_iterator{end};
        const auto rend = std::reverse_iterator{begin};

        const auto folded_flags_l = std::accumulate(begin, end, flags{}, std::bit_or<flags>{});
        const auto folded_flags_r = std::accumulate(rbegin, rend, flags{}, std::bit_or<flags>{});
        REQUIRE(folded_flags_l == folded_flags_r);

        static const auto bit_or_assign = [](flags& value, const flags rhs) { value |= rhs; };
        const auto mutated_flags_l = mutate_for_each(begin, end, flags{}, bit_or_assign);
        const auto mutated_flags_r = mutate_for_each(rbegin, rend, flags{}, bit_or_assign);
        REQUIRE(mutated_flags_l == mutated_flags_r);
        REQUIRE(folded_flags_l == mutated_flags_l);

        // Note the use of REQUIRE() rather than CHECK() above: we have here confirmed that all
        // methods of combining the flags for the current permutation are equivalent, and can
        // proceed by choosing any of those.

        const auto& combined_flags = folded_flags_l;
        for (auto&& flag : flag_domain) {
            const auto has_flag = ((combined_flags & flag) == flag);
            CHECK(has_flag == contains(begin, end, flag));
        }
    }
}

TEST_CASE("flag_operations", "[flag_product]") {
    sub_permute(flag_domain, check_permutation);
}
