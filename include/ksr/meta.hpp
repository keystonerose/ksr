#ifndef KSR_META_HPP
#define KSR_META_HPP

#include "meta_tag.hpp"

#include <cstddef>
#include <type_traits>

namespace ksr { namespace meta {


    template <typename func_seq_t, typename arg_seq_t, typename = >
    constexpr auto apply(const func_seq_t func_seq, const arg_seq_t arg_seq) {

        static_assert(!empty(func_seq));
        if constexpr (size(func_seq) == 1) {
            return type_tag<func_seq.head.func<>>{}; // TODO:HERE
        } else {
            return func_seq.head of apply(func_seq.tail, arg_seq);
        }
    }

    /// Instantiates a specified template `func` that takes either type or non-type parameters using
    /// the corresponding elements of the sequence `seq`.

    template <template <typename...> class func, typename... ts>
    constexpr auto apply(type_seq<ts...>) {
        return type_tag<func<ts...>>{};
    }

    template <template <auto...> class func, auto... vs>
    constexpr auto apply(value_seq<vs...>) {
        return type_tag<func<vs...>>{};
    }

    template <typename func_tag_t, typename arg_seq_t>
    constexpr auto invoke(func_tag_t, arg_seq_t) {

    }

    // TODO:DOC
    // TODO:HERE

    template <template <typename...> class funcs_head, template <typename...> class... funcs_tail>
    auto invoke(type_seq

    struct invoke {
        template <typename... args>
        using type = head<typename compose<tail...>::template func<args...>>;
    };

    /// Instantiates a specified template `func` taking a single type or non-type template parameter
    /// for each corresponding element of the sequence `seq`, returning a `type_seq` object tagging
    /// the sequence of `func` instantiations.

    template <template <typename> class func, typename... ts>
    constexpr auto transform_by(type_seq<ts...>) {
        return type_seq<func<ts>...>{};
    }

    template <template <auto> class func, auto... vs>
    constexpr auto transform_by(value_seq<vs...>) {
        return type_seq<func<vs>...>{};
    }

    namespace detail { namespace concat {{

        /// Implements the functionality of `concat()` by recursively appending each element of
        /// `rhs` to `lhs`. Results in fewer template instantiations than `concat_front()` when
        /// `rhs` is shorter than `lhs`.

        template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
        constexpr auto concat_back(const lhs_t lhs, const rhs_t rhs) {

            if constexpr (empty(rhs)) {
                return lhs;
            } else {
                return concat_back(push_back(lhs, rhs.head), rhs.tail);
            }
        }

        /// Implements the functionality of `concat()` by recursively prepending each element of
        /// `lhs` to `rhs`. Results in fewer template instantiations than `concat_back()` when `lhs`
        /// is shorter than `rhs`.

        template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
        constexpr auto concat_front(const lhs_t lhs, const rhs_t rhs) {

            if constexpr (empty(lhs)) {
                return rhs;
            } else {
                return push_front(concat(lhs.tail, rhs), lhs.head);
            }
        }
    }}

    /// Concatenates `lhs` and `rhs` by recursively pushing elements of the shorter operand sequence
    /// onto the longer one. `lhs` and `rhs` must be sequence types of the same kind in the sense of
    /// the `is_seq` trait.

    template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
    constexpr auto concat(const lhs_t lhs, const rhs_t rhs) {

        if constexpr (size(lhs) <= size(rhs)) {
            return detail::concat_front(lhs, rhs);
        } else {
            return detail::concat_back(lhs, rhs);
        }
    }

    /// Calls `visitor` for each item in `seq`, passed as a tagged object (that is, a `type_tag` or
    /// `value_tag`, depending on the kind of `seq_t`). `seq_t` must be a sequence type in the sense
    /// of the `is_seq` trait, and `visitor_t` must be a callable type taking a single argument of
    /// any tag type in `seq_t`.

    template <typename seq_t, typename visitor_t, typename = std::enable_if_t<is_seq_v<seq_t>>>
    constexpr void for_each(const seq_t seq, [[maybe_unused]] const visitor_t visitor) {

        if constexpr (!empty(seq)) {
            visitor(seq.head);
            for_each(seq.tail, visitor);
        }
    }

    /// Determines whether `lhs` is a subsequence of `lhs`. Both `lhs_t` and `rhs_t` must be
    /// sequence types of the same kind in the sense of the `is_seq` trait. Returns `true` if both
    /// `lhs` and `rhs` are empty.

    template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
    constexpr auto subseq(const lhs_t lhs, [[maybe_unused]] const rhs_t rhs) -> bool {

        if constexpr (empty(lhs)) {
            return true;
        } else {
            if constexpr (empty(rhs)) {
                return false;
            } else if constexpr (lhs.head == rhs.head) {
                return subseq(lhs.tail, rhs.tail);
            } else {
                return subseq(lhs, rhs.tail);
            }
        }
    }

    /// Determines whether `seq` contains a specified tagged type or integral constant (as
    /// appropriate to the kind of `seq`). While this is a special case of `subseq()`, note the
    /// reversed argument order (to fit the left-to-right reading of the predicate name).

    template <typename item_t, typename... seq_ts>
    constexpr auto contains(const type_seq<seq_ts...> seq, type_tag<item_t> = {}) -> bool {
        return subseq(type_seq<item_t>{}, seq);
    }

    template <auto item_v, auto... seq_vs>
    constexpr auto contains(const value_seq<seq_vs...> seq, value_tag<item_v> = {}) -> bool {
        return subseq(value_seq<item_v>{}, seq);
    }
}}

#endif
