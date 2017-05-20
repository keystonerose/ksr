#ifndef KSR_FUNCTIONAL_HPP
#define KSR_FUNCTIONAL_HPP

#include "type_traits.hpp"

#include <functional>
#include <type_traits>

namespace ksr {

    /// Function object adaptor that applies a default-constructed instance of `pred` to specified
    /// members of a particular class type.
    ///
    /// For example, if `agg` is defined as
    /// ```c++
    /// struct agg {
    ///     int value;
    /// };
    /// ```
    /// then `mem_pred<&agg::value, std::less<int>>{}` is a function object specifying an ordering
    /// for `agg` objects based upon the corresponding ordering of their `values` members in the
    /// sense of `std::less`.
    ///
    /// Aliases of `mem_pred` specifying the value of the `pred` parameter are provided for the
    /// standard library comparison function objects (`mem_less` in the example above).

    template <auto mem_ptr, typename pred>
    struct mem_pred;

    template <typename mem_t, typename class_t, mem_t class_t::* mem_ptr, typename pred>
    struct mem_pred<mem_ptr, pred> {

        template <typename... arg_ts>
        constexpr auto operator()(const arg_ts&... args) const
            -> std::enable_if_t<is_same_v<class_t, arg_ts...>, bool> {
            return pred{}(args.*mem_ptr...);
        }
    };

    template <auto mem_ptr>
    using mem_equal_to = mem_pred<mem_ptr, std::equal_to<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_not_equal_to = mem_pred<mem_ptr, std::not_equal_to<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_greater = mem_pred<mem_ptr, std::greater<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_less = mem_pred<mem_ptr, std::less<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_greater_equal = mem_pred<mem_ptr, std::greater_equal<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_less_equal = mem_pred<mem_ptr, std::less_equal<mem_type_t<mem_ptr>>>;

    /// Function object adaptor that applies a default-constructed instance of `pred` to a value
    /// that the `current_mem_pred` object was constructed and specified members of the arguments
    /// passed to `operator()`. `mem_ptr` should be a pointer to the member to pass to the `pred`
    /// instance.
    ///
    /// For example, if `agg` is defined as
    /// ```c++
    /// struct agg {
    ///     int value;
    /// };
    /// ```
    /// then `curried_mem_pred<&agg::value, std::equal_to<int>>{0}` is a predicate on `agg` objects
    /// that returns `true` iff the `value` member of its argument is equal to zero in the sense of
    /// `std::equal_to`.
    ///
    /// Aliases of `curried_mem_pred` specifying the value of the `pred` parameter are provided for
    /// the standard library comparison function objects (`mem_is_equal_to` in the example above).
    ///
    /// Note that `curried_mem_pred` is implemented by a stand-alone class template rather than a
    /// generalised currying mechanism aplied to `mem_pred` because the curried argument is not of
    /// the class type passed as the first argument to `mem_pred::operator()` but rather of the
    /// member type.

    template <auto mem_ptr, typename pred>
    struct curried_mem_pred;

    template <typename mem_t, typename class_t, mem_t class_t::* mem_ptr, typename pred>
    struct curried_mem_pred<mem_ptr, pred> {
    public:

        constexpr explicit curried_mem_pred(const mem_t& head)
            : m_head{head} {}

        template <typename... tail_ts>
        constexpr auto operator()(const tail_ts&... tail) const
            -> std::enable_if_t<is_same_v<class_t, tail_ts...>, bool> {
            return pred{}(m_head, tail.*mem_ptr...);
        }

    private:
        std::reference_wrapper<const mem_t> m_head;
    };

    template <auto mem_ptr>
    using mem_is_equal_to = curried_mem_pred<mem_ptr, std::equal_to<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_is_not_equal_to = curried_mem_pred<mem_ptr, std::not_equal_to<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_is_greater = curried_mem_pred<mem_ptr, std::greater<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_is_less = curried_mem_pred<mem_ptr, std::less<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_is_greater_equal = curried_mem_pred<mem_ptr, std::greater_equal<mem_type_t<mem_ptr>>>;

    template <auto mem_ptr>
    using mem_is_less_equal = curried_mem_pred<mem_ptr, std::less_equal<mem_type_t<mem_ptr>>>;
}

#endif
