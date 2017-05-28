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

    enum class bits : std::uint8_t {
        bit_1 = (1 << 0),
        bit_2 = (1 << 1),
        bit_3 = (1 << 2)
    };

    KSR_DEFINE_BIT_OPERATIONS(bits)
    auto bit_domain = std::array{bits::bit_1, bits::bit_2, bits::bit_3};
    using bit_domain_iter = decltype(bit_domain)::iterator;

    /// Given a range of `bits` codes describing a particular permutation of options, verifies that
    /// combining those bits using various bitwise-or operations gives a compound value that can be
    /// subsequently decomposed into each individual input bits (and only those input bits). More
    /// formally, checks that the `|` and `|=` operators for `bits` behave in a consistent and
    /// commutative way, and that the `&` operator provides the inverse operation.

    void check_bit_permutation(const bit_domain_iter begin, const bit_domain_iter end) {

        const auto rbegin = std::reverse_iterator{end};
        const auto rend = std::reverse_iterator{begin};

        const auto folded_bits_l = std::accumulate(begin, end, bits{}, std::bit_or<bits>{});
        const auto folded_bits_r = std::accumulate(rbegin, rend, bits{}, std::bit_or<bits>{});
        REQUIRE(folded_bits_l == folded_bits_r);

        static const auto bit_or_assign = [](bits& value, const bits rhs) { value |= rhs; };
        const auto mutated_bits_l = mutate_for_each(begin, end, bits{}, bit_or_assign);
        const auto mutated_bits_r = mutate_for_each(rbegin, rend, bits{}, bit_or_assign);
        REQUIRE(mutated_bits_l == mutated_bits_r);
        REQUIRE(folded_bits_l == mutated_bits_l);

        // Note the use of REQUIRE() rather than CHECK() above: we have here confirmed that all
        // methods of combining the bits for the current permutation are equivalent, and can
        // proceed by choosing any of those.

        const auto& combined_bits = folded_bits_l;
        for (auto&& bit : bit_domain) {
            const auto has_bit = ((combined_bits & bit) == bit);
            CHECK(has_bit == contains(begin, end, bit));
        }
    }
}

TEST_CASE("bit_operations", "[flag_product]") {
    sub_permute(bit_domain, check_bit_permutation);
}
