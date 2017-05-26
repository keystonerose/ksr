#ifndef KSR_META_HPP
#define KSR_META_HPP

#include "meta_type_traits.hpp"

#include <cstddef>
#include <type_traits>

namespace ksr { namespace meta {

    /// `type_tag` and `value_tag` wrap single types or integral constants, respectively, in objects
    /// that facilitate the deduction of corresponding template parameters and can be both be passed
    /// to or returned from `constexpr` functions. The **kind** of such a tag object designates
    /// whether it wraps a type or a value.

    /// Tags a single type `t`, which can later be retrieved from the `type_tag` object using
    /// `decltype` and the `type` member type.

    template <typename t>
    struct type_tag {
        using type = t;
    };

    /// Determines whether two `type_tag` objects represent the same type, in the sense of
    /// `std::is_same`.

    template <typename lhs_t, typename rhs_t>
    constexpr auto operator==(type_tag<lhs_t>, type_tag<rhs_t>) -> bool {
        return std::is_same_v<lhs_t, rhs_t>;
    }

    template <typename lhs_t, typename rhs_t>
    constexpr auto operator!=(const type_tag<lhs_t> lhs, const type_tag<rhs_t> rhs) -> bool {
        return !(lhs == rhs);
    }

    /// Tags an integral constant value `v`, which can later be retrieved via the `value` member.
    /// While integral constants can of course be managed more naturally without the tagging
    /// mechanism, this is necessary in some cases to facilitate the deduction of a corresponding
    /// template parameter. `value_tag` objects are considered identical when they represent values
    /// that themselves compare equal with `operator==`.

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

    /// `type_seq` and `value_seq are sequence counterparts of `type_tag` and `value_tag`: each
    /// wraps a pack of arbitrary types or integral constants and provides operations for processing
    /// that pack in terms of tag objects of the corresponding kind. Sequence objects that tag
    /// non-empty packs are typically processed by splitting into a head (which is a tagged type or
    /// or value) and a tail (which is another sequence, and may be empty) via the `head` and `tail`
    /// members. These components are available only when the pack is non-empty; whether or not this
    /// is the case can be determined via the nonmember `empty()` predicate.

    template <typename...>
    struct type_seq;

    template <>
    struct type_seq<> {};

    template <typename head_t, typename... tail_ts>
    struct type_seq<head_t, tail_ts...> {
        static constexpr auto head = type_tag<head_t>{};
        static constexpr auto tail = type_seq<tail_ts...>{};
    };

    template <typename... ts>
    constexpr auto size(type_seq<ts...>) -> std::size_t {
        return sizeof...(ts);
    }

    template <typename item_t, typename... seq_ts>
    constexpr auto push_back(type_seq<seq_ts...>, type_tag<item_t> = {}) {
        return type_seq<seq_ts..., item_t>{};
    }

    template <typename item_t, typename... seq_ts>
    constexpr auto push_front(type_seq<seq_ts...>, type_tag<item_t> = {}) {
        return type_seq<item_t, seq_ts...>{};
    }

    template <auto...>
    struct value_seq;

    template <>
    struct value_seq<> {};

    template <auto head_v, auto... tail_vs>
    struct value_seq<head_v, tail_vs...> {
        static constexpr auto head = value_tag<head_v>{};
        static constexpr auto tail = value_seq<tail_vs...>{};
    };

    template <auto... vs>
    constexpr auto size(value_seq<vs...>) -> std::size_t {
        return sizeof...(vs);
    }

    template <auto item_v, auto... seq_vs>
    constexpr auto push_back(value_seq<seq_vs...>, value_tag<item_v> = {}) {
        return value_seq<seq_vs..., item_v>{};
    }

    template <auto item_v, auto... seq_vs>
    constexpr auto push_front(value_seq<seq_vs...>, value_tag<item_v> = {}) {
        return value_seq<item_v, seq_vs...>{};
    }

    /// Trait used to constrain templates that operate upon the sequence types defined in
    /// `ksr::meta`. Evaluates to `std::true_type` if every type in `ts` is a `type_seq`
    /// instantiation or every type in `ts` is a `value_seq` instantiation, or to `std::false_type`
    /// otherwise.

    template <typename... ts>
    struct is_seq : std::bool_constant<
        std::conjunction_v<is_type_inst<ts, type_seq>...> ||
        std::conjunction_v<is_value_inst<ts, value_seq>...>> {};

    template <typename... ts>
    inline constexpr auto is_seq_v = is_seq<ts...>::value;

    /// Instantiates a specified template `op` that takes either type or non-type parameters using
    /// the corresponding elements of the sequence `seq`, and makes the resulting type available
    /// through the `type` member.

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

    /// Instantiates a specified template `op` taking a single type or non-type template parameter
    /// for each corresponding element of the sequence `seq`, and makes the resulting sequence of
    /// types available through the `type` member (which will itself be an instantiation of
    /// `type_seq`). This is a metaprogramming analog of the `std::transform()` algorithm, and the
    /// naming and parameter order reflects this.

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

    /// Determines whether `seq` is an empty sequence. Nonempty sequences provide `head` and `tail`
    /// members. `seq_t` must be a sequence type in the sense of the `is_seq` trait.

    template <typename seq_t, typename = std::enable_if_t<is_seq_v<seq_t>>>
    constexpr auto empty(const seq_t seq) -> bool {
        return size(seq) == 0;
    }

    namespace detail {

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
    }

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
    constexpr void for_each(const seq_t seq, const visitor_t visitor) {

        if constexpr (!empty(seq)) {
            visitor(seq.head);
            for_each(seq.tail, visitor);
        }
    }

    /// Determines whether `lhs` is a subsequence of `lhs`. Both `lhs_t` and `rhs_t` must be
    /// sequence types of the same kind in the sense of the `is_seq` trait. Returns `true` if both
    /// `lhs` and `rhs` are empty.

    template <typename lhs_t, typename rhs_t, typename = std::enable_if_t<is_seq_v<lhs_t, rhs_t>>>
    constexpr auto subseq(const lhs_t lhs, const rhs_t rhs) -> bool {

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
