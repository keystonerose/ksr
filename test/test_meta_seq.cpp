#include "ksr/meta.hpp"

#include <cstddef>
#include <cstdint>

using namespace ksr;
using namespace meta;

namespace {

    template <typename seq_t>
    constexpr auto size(const seq_t seq) -> std::size_t {
        auto result = std::size_t{0};
        for_each(seq, [&result](auto) { ++result; });
        return result;
    }

    template <typename seq_t>
    constexpr auto sum(const seq_t seq) -> int {
        auto result = 0;
        for_each(seq, [&result](const auto item) { result += item; });
        return result;
    }

    constexpr auto empty_vals = value_seq<>{};
    constexpr auto homo_vals = value_seq<0, 1, 2, 3>{};
    constexpr auto hetero_vals = value_seq<0, 1u, std::int64_t{2}, std::uint8_t{3}>{};

    static_assert(sum(empty_vals) == 0);
    static_assert(sum(homo_vals) == 6);
    static_assert(sum(hetero_vals) == 6);

    static_assert(!contains(empty_vals, 1));
    static_assert(contains(homo_vals, 1));
    static_assert(contains(hetero_vals, 1));
}
