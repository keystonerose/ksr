#include "ksr/meta.hpp"

using namespace ksr;
using namespace meta;

namespace test { namespace {

    template <typename seq_t>
    constexpr auto sum(const seq_t seq) -> int {
        auto result = 0;
        for_each(seq, [&result](const auto item) { result += item.value; });
        return result;
    }
}}

namespace {

    constexpr auto empty = value_seq<>{};
    constexpr auto seq = value_seq<2, 3, 5>{};

    static_assert(test::sum(empty) == 0);
    static_assert(test::sum(seq) == 10);

    static_assert(!contains<-1>(empty));
    static_assert(!contains<0>(empty));
    static_assert(!contains<1>(empty));
    static_assert(!contains<2>(empty));

    static_assert(!contains<-1>(seq));
    static_assert(!contains<0>(seq));
    static_assert(!contains<1>(seq));
    static_assert(contains<2>(seq));
    static_assert(contains<3>(seq));
    static_assert(!contains<4>(seq));
    static_assert(contains<5>(seq));
    static_assert(!contains<6>(seq));

    constexpr auto subseq_left = value_seq<2, 3>{};
    constexpr auto subseq_right = value_seq<3, 5>{};
    constexpr auto subseq_outer = value_seq<2, 5>{};

    static_assert(subseq(empty, empty));
    static_assert(subseq(empty, seq));
    static_assert(subseq(seq, seq));
    static_assert(!subseq(seq, empty));

    static_assert(subseq(subseq_left, seq));
    static_assert(subseq(subseq_right, seq));
    static_assert(subseq(subseq_outer, seq));

    static_assert(!subseq(seq, subseq_left));
    static_assert(!subseq(seq, subseq_right));
    static_assert(!subseq(seq, subseq_outer));

    static_assert(subseq(subseq_left, subseq_left));
    static_assert(subseq(subseq_right, subseq_right));
    static_assert(subseq(subseq_outer, subseq_outer));

    static_assert(!subseq(subseq_left, subseq_right));
    static_assert(!subseq(subseq_left, subseq_outer));
    static_assert(!subseq(subseq_right, subseq_left));
    static_assert(!subseq(subseq_right, subseq_outer));
    static_assert(!subseq(subseq_outer, subseq_left));
    static_assert(!subseq(subseq_outer, subseq_right));
}
