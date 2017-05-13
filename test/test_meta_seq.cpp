#include "ksr/meta.hpp"

#include <cstddef>

using namespace ksr;
using namespace meta;

namespace {

    template <typename... ts>
    constexpr auto type_seq = ksr::meta::type_seq<ts...>{};

    template <auto... vs>
    constexpr auto value_seq = ksr::meta::value_seq<vs...>{};

    template <typename seq_t>
    constexpr auto size(const seq_t seq) -> std::size_t {
        auto result = std::size_t{0};
        ksr::meta::for_each(seq, [&result](auto) { ++result; });
        return result;
    }

    template <typename seq_t>
    constexpr auto sum(const seq_t seq) -> int {
        auto result = 0;
        ksr::meta::for_each(seq, [&result](const auto item) { result += item; });
        return result;
    }

    static_assert(

    constexpr void test_for_each() {

        auto values = meta::value_seq<0, 1u, 2l, 3>;

        auto sum = 0;

    }

    using values = meta::value_seq<0, 1, 2>;

    static_assert();
}
