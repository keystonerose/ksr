#ifndef KSR_META_HPP
#define KSR_META_HPP

#include "meta_type_traits.hpp"
#include <type_traits>

namespace ksr { namespace meta {

    ///
    /// Wraps a single type \p t in an object that can be passed to or returned from `constexpr`
    /// functions performing compile-time operations on types. The tagged type can be retrieved from
    /// a \ref type_tag object using `decltype` and the `type` member type. \ref type_tag objects
    /// have no state, but are considered identical when they represent the same type (in the sense
    /// of `std::is_same`).
    ///

    template <typename t>
    struct type_tag {
        using type = t;
    };

    template <typename lhs_t, typename rhs_t>
    constexpr auto operator==(type_tag<lhs_t>, type_tag<rhs_t>) -> bool {
        return std::is_same_v<lhs_t, rhs_t>;
    }

    template <typename lhs_t, typename rhs_t>
    constexpr auto operator!=(const type_tag<lhs_t> lhs, const type_tag<rhs_t> rhs) -> bool {
        return !(lhs == rhs);
    }

    ///
    /// Wraps an integral constant value \p v in an object that can be passed as an argument to
    /// functions to facilitate the deduction of a corresponding template parameter. The tagged
    /// value can be retrieved via the `value` member. \ref value_tag objects have no state, but are
    /// considered identical when the values they tag are equal.
    ///

    template <auto v>
    struct value_tag {
        static constexpr auto value = v;
    };

    template <auto lhs_v, auto rhs_v>
    constexpr auto operator==(value_tag<lhs_v>, value_tag<rhs_v>) -> bool {
        return lhs_v == rhs_v;
    }

    template <auto lhs_v, auto rhs_v>
    constexpr auto operator!=(const value_tag<lhs_v> lhs, const value_tag<rhs_v> rhs) -> bool {
        return !(lhs == rhs);
    }

    // TODO:DOC
    // define "item types" -> types of the values in a value_seq; tag instances in a type_seq
    // define "type codomain"
    // TODO adapt the below comment

    ///
    /// Wraps a pack of arbitrary types and provides operations for processing that pack in terms of
    /// (presumably \c constexpr) \ref type_tag objects. \ref type_seq objects tagging a non-empty
    /// \p ts pack are typically processed by splitting into a `head` (which is a \ref type_tag
    /// representing the first type in \p ts) and a `tail` (which is another \ref type_seq
    /// representing the following types in \p ts, and may be empty). These operations are available
    /// via the members of those names when \p ts is non-empty, but are not present when it is
    /// empty; whether or not this is the case is indicated by the `empty` member, which is present
    /// in both cases.
    ///

    template <typename...>
    struct type_seq;

    template <>
    struct type_seq<> {
        static constexpr auto empty = true;
    };

    template <typename head_t, typename... tail_ts>
    struct type_seq<head_t, tail_ts...> {
        static constexpr auto empty = false;
        static constexpr auto head = type_tag<head_t>{};
        static constexpr auto tail = type_seq<tail_ts...>{};
    };

    template <typename item_t, typename... seq_ts>
    constexpr auto push_back(type_seq<seq_ts...>, type_tag<item_t>) {
        return type_seq<seq_ts..., item_t>{};
    }

    template <typename item_t, typename... seq_ts>
    constexpr auto push_front(type_seq<seq_ts...>, type_tag<item_t>) {
        return type_seq<item_t, seq_ts...>{};
    }

    // TODO:DOC

    template <auto...>
    struct value_seq;

    template <>
    struct value_seq<> {
        static constexpr auto empty = true;
    };

    template <auto head_v, auto... tail_vs>
    struct value_seq<head_v, tail_vs...> {
        static constexpr auto empty = false;
        static constexpr auto head = value_tag<head_v>{};
        static constexpr auto tail = value_seq<tail_vs...>{};
    };

    template <auto item_v, auto... seq_vs>
    constexpr auto push_back(value_seq<seq_vs...>, value_tag<item_v> = {}) {
        return value_seq<seq_vs..., item_v>{};
    }

    template <auto item_v, auto... seq_vs>
    constexpr auto push_front(value_seq<seq_vs...>, value_tag<item_v> = {}) {
        return value_seq<item_v, seq_vs...>{};
    }

    ///
    /// Trait used to constrain templates that operate upon sequence types defined in
    /// \ref ksr::meta. Evaluates to `std::true_type` if every type in \p ts is a \ref type_seq
    /// instantiation or every type in \p ts is a \ref value_seq instantiation, or to
    /// `std::false_type` otherwise.
    ///

    template <typename... ts>
    struct is_seq : std::bool_constant<
        std::conjunction_v<is_type_inst<ts, type_seq>...> ||
        std::conjunction_v<is_value_inst<ts, value_seq>...>> {};

    template <typename... ts>
    inline constexpr auto is_seq_v = is_seq<ts...>::value;

    // TODO:DOC

    template <typename seq, template <typename...> class op>
    struct apply_types {};

    template <template <typename...> class op, typename... ts>
    struct apply_types<type_seq<ts...>, op> {
        using type = op<ts...>;
    };

    template <typename seq, template <typename...> class op>
    using apply_types_t = typename apply_types<seq, op>::type;

    template <typename seq, template <auto...> class op>
    struct apply_values {};

    template <template <auto...> class op, auto... vs>
    struct apply_values<value_seq<vs...>, op> {
        using type = op<vs...>;
    };

    template <typename seq, template <auto> class op>
    using apply_values_t = typename apply_values<seq, op>::type;

    // TODO:DOC
    // argument order mirrors std::transform

    template <typename seq, template <typename> class op>
    struct transform_types {};

    template <template <typename> class op, typename... ts>
    struct transform_types<type_seq<ts...>, op> {
        using type = type_seq<op<ts>...>;
    };

    template <typename seq, template <typename> class op>
    using transform_types_t = typename transform_types<seq, op>::type;

    template <typename seq, template <auto> class op>
    struct transform_values {};

    template <template <auto> class op, auto... vs>
    struct transform_values<value_seq<vs...>, op> {
        using type = type_seq<op<vs>...>;
    };

    template <typename seq, template <auto> class op>
    using transform_values_t = typename transform_values<seq, op>::type;

    ///
    /// Calls \p visitor for each item in \p seq. \p seq_t must be a sequence in the sense of the
    /// \ref is_seq trait, and \p visitor_t must be a callable type taking a single argument of any
    /// item type in \p seq_t.
    ///

    template <typename seq_t, typename visitor_t, typename = std::enable_if_t<is_seq_v<seq_t>>>
    constexpr void for_each(const seq_t seq, const visitor_t visitor) {

        if constexpr (!seq.empty) {
            visitor(seq.head);
            for_each(seq.tail, visitor);
        }
    }

    // TODO:DOC
    ///
    /// Determines whether \p haystack contains the item \p needle at any position. \p seq_t must
    /// be a sequence in the sense of the \ref is_seq trait. If \p seq_t is a type sequence, then
    /// \p item_t must be an instantiation of \ref type_tag; if it is a value sequence, \p item_t
    /// must be a type comparable to every item type in \p seq_t.
    ///

    template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
    constexpr auto subseq(const lhs_t lhs, const rhs_t rhs) -> bool {

        if constexpr (lhs.empty) {
            return true;
        } else {
            if constexpr (rhs.empty) {
                return false;
            } else if constexpr (rhs.head == lhs.head) {
                return subseq(rhs.tail, lhs.tail);
            } else {
                return subseq(rhs.tail, lhs);
            }
        }
    }

    // TODO:DOC

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
