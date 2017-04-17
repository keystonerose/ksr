#ifndef KSR_META_HPP
#define KSR_META_HPP

#include <type_traits>

namespace ksr { namespace meta {

    ///
    /// Wraps a single type \p t in an object that can be passed to or returned from \c constexpr
    /// functions performing compile-time operations on types. The tagged type can be retrieved from
    /// a \ref tag object using \c decltype and the \c type member type. \ref tag objects have no
    /// state, but are considered identical when they represent the same type (in the sense of
    /// \c std::is_same).
    ///

    template <typename t>
    struct tag {
        using type = t;
    };

    template <typename lhs_t, typename rhs_t>
    constexpr auto operator==(tag<lhs_t>, tag<rhs_t>) -> bool {
        return std::is_same_v<lhs_t, rhs_t>;
    }

    template <typename lhs_t, typename rhs_t>
    constexpr auto operator!=(const tag<lhs_t> lhs, const tag<rhs_t> rhs) -> bool {
        return !(lhs == rhs);
    }

    ///
    /// Wraps a pack of arbitrary types and provides operations for processing that pack in terms of
    /// (presumably \c constexpr) \ref tag objects. \ref type_seq objects tagging a non-empty
    /// \p ts pack are typically processed by splitting into a head (which is a \ref tag
    /// representing the first type in \p ts) and a \c tail (which is another \ref type_seq
    /// representing the following types in \p ts, and may be empty). These operations are available
    /// via the members of those names when \p ts is non-empty, but are not present when it is
    /// empty; whether or not this is the case is indicated by the \c empty member, which is present
    /// in both cases.
    ///

    template <typename... ts>
    struct type_seq;

    template <>
    struct type_seq<> {
        static constexpr auto empty = true;
    };

    template <typename head_t, typename... tail_ts>
    struct type_seq<head_t, tail_ts...> {
        static constexpr auto empty = false;
        static constexpr auto head = tag<head_t>{};
        static constexpr auto tail = type_seq<tail_ts...>{};
    };

    ///
    /// Determines whether the type sequence \p haystack contains the type tagged by \p needle at
    /// any position.
    ///

    template <typename needle_t, typename... haystack_ts>
    constexpr auto contains(
        const type_seq<haystack_ts...> haystack, const tag<needle_t> needle) -> bool {

        if constexpr (haystack.empty) {
            return false;
        } else if constexpr (haystack.head == needle) {
            return true;
        } else {
            return contains(haystack.tail, needle);
        }
    }

    ///
    /// Determines whether the types tagged by \p lhs and \p rhs both occur within the type sequence
    /// \p ordering, with \p lhs occurring strictly before \p rhs.
    ///

    template <typename lhs_t, typename rhs_t, typename... ordering_ts>
    constexpr auto less(
        const type_seq<ordering_ts...> ordering,
        const tag<lhs_t> lhs, const tag<rhs_t> rhs) -> bool {

        if constexpr (ordering.empty) {
            return false;
        } else if constexpr (ordering.head == lhs) {
            return contains(ordering.tail, rhs);
        } else {
            return less(ordering.tail, lhs, rhs);
        }
    }
}}

#endif
