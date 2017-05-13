#ifndef KSR_META_HPP
#define KSR_META_HPP

#include <type_traits>

namespace ksr { namespace meta {

    // TODO:DOC

    template <typename t, template <typename...> class template_>
    struct is_type_inst : std::false_type {};

    template <template <typename...> class template_, typename... ts>
    struct is_type_inst<template_<ts...>, template_> : std::true_type {};

    template <typename t, template <typename...> class template_>
    inline constexpr auto is_type_inst_v = is_type_inst<t, template_>::value;

    // TODO:DOC

    template <typename t, template <auto...> class template_>
    struct is_value_inst : std::false_type {};

    template <template <auto...> class template_, auto... vs>
    struct is_value_inst<template_<vs...>, template_> : std::true_type {};

    template <typename t, template <auto...> class template_>
    inline constexpr auto is_value_inst_v = is_value_inst<t, template_>::value;

    ///
    /// Wraps a single type \p t in an object that can be passed to or returned from `constexpr`
    /// functions performing compile-time operations on types. The tagged type can be retrieved from
    /// a \ref tag object using `decltype` and the `type` member type. \ref tag objects have no
    /// state, but are considered identical when they represent the same type (in the sense of
    /// `std::is_same`).
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

    // TODO:DOC
    // define "item types" -> types of the values in a value_seq; tag instances in a type_seq
    // define "type codomain"
    // TODO adapt the below comment

    ///
    /// Wraps a pack of arbitrary types and provides operations for processing that pack in terms of
    /// (presumably \c constexpr) \ref tag objects. \ref type_seq objects tagging a non-empty
    /// \p ts pack are typically processed by splitting into a `head` (which is a \ref tag
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
        static constexpr auto head = tag<head_t>{};
        static constexpr auto tail = type_seq<tail_ts...>{};
    };

    template <typename item_t, typename... seq_ts>
    constexpr auto push_back(type_seq<seq_ts...>, tag<item_t>) {
        return type_seq<seq_ts..., item_t>{};
    }

    template <typename item_t, typename... seq_ts>
    constexpr auto push_front(type_seq<seq_ts...>, tag<item_t>) {
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
        static constexpr auto head = head_v;
        static constexpr auto tail = value_seq<tail_vs...>{};
    };

    template <typename item_t, auto... seq_vs>
    constexpr auto push_back(value_seq<seq_vs...>, const item_t item) {
        return value_seq<seq_vs..., item>{};
    }

    template <typename item_t, auto... seq_vs>
    constexpr auto push_front(value_seq<seq_vs...>, const item_t item) {
        return value_seq<item, seq_vs...>{};
    }

    // TODO:DOC if seq_t is not a type_seq or value_seq, gives a substitution failure

    template <typename seq_t, typename t>
    struct codomain {};

    template <typename t, typename... seq_ts>
    struct codomain<type_seq<seq_ts...>, t> {
        using type = tag<t>;
    };

    template <typename t, auto... seq_vs>
    struct codomain<value_seq<seq_vs...>, t> {
        using type = t;
    };

    template <typename seq_t, typename t>
    using codomain_t = typename codomain<seq_t, t>::type;

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

    template <typename t>
    inline constexpr auto is_seq_v = is_seq<t>::value;

    // TODO:DOC

    template <typename seq, template <auto> class op>
    struct apply;

    template <template <auto> class op, auto... vs>
    struct apply<value_seq<vs...>, op> {
        using type = op<vs...>;
    };

    template <template <auto> class op, typename... ts>
    struct apply<type_seq<ts...>, op> {
        using type = op<ts...>;
    };

    template <typename seq, typename <auto> class op>
    using apply_t = typename apply<seq, op>::type;

    // TODO:DOC
    // argument order mirrors std::transform

    template <typename seq, template <auto> class op>
    struct transform;

    template <template <auto> class op, auto... vs>
    struct transform<value_seq<vs...>, op> {
        using type = type_seq<op<vs>...>;
    };

    template <template <auto> class op, typename... ts>
    struct transform<type_seq<ts...>, op> {
        using type = type_seq<op<ts>...>;
    };

    template <typename seq, typename <auto> class op>
    using transform_t = typename transform<seq, op>::type;

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
    /// \p item_t must be an instantiation of \ref tag; if it is a value sequence, \p item_t must be
    /// a type comparable to every item type in \p seq_t.
    ///

    template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
    constexpr auto subseq(const lhs_t lhs, const rhs_t rhs) -> bool {

        if constexpr (lhs.empty) {
            return true;
        } else {
            if constexpr (rhs.empty) {
                return false;
            } else if constexpr (rhs.head == lhs.head) {
                return search(rhs.tail, lhs.tail);
            } else {
                return search(rhs.tail, lhs);
            }
        }
    }

    // TODO:DOC

    template <typename seq_t, typename t>
    constexpr auto contains(const seq_t seq, const codomain_t<seq_t, t> item) -> bool {

        if constexpr (is_type_inst_v<seq_t, type_seq>>) {
            return subseq(seq, type_seq<item_t::type>{});
        } else {
            return subseq(seq, value_seq<item>{});
        }
    }
}}

#endif
