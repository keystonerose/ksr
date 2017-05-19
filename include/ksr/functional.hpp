#ifndef KSR_FUNCTIONAL_HPP
#define KSR_FUNCTIONAL_HPP

#include "type_traits.hpp"

#include <functional>
#include <type_traits>
#include <utility>

namespace ksr {

    /// Function object that determines whether a particular member of an aggregate is equal to (in
    /// the sense of `std::equal_to`) the value that the `mem_equals` object was constructed with.
    /// `mem_ptr` should be a pointer to the member to test.

    template <auto mem_ptr>
    struct mem_equals;

    template <typename mem_t, typename t, mem_t t::* mem_ptr>
    struct mem_equals<mem_ptr> {
    public:

        explicit mem_equals(const mem_t& value)
            : m_value{value} {}

        constexpr auto operator()(const t& rhs) const -> bool {
            return std::equal_to<mem_t>{}(m_value, rhs.*mem_ptr);
        }

    private:
        std::reference_wrapper<const mem_t> m_value;
    };

    /// Function object adaptor that applies an appropriate instantiation of a predicate function
    /// object template to specified members of a particular aggregate type. For example, for an
    /// aggregate type `agg` with a member `value`, the class `mem_pred<&agg::value, std::less>`
    /// is a function object specifying an ordering for `agg` objects based upon the corresponding
    /// ordering of their `values` members in the sense of `std::less`. (Aliases are provided for
    /// the standard library comparison function objects, in this case `mem_less`.)

    template <auto mem_ptr, template <typename> class pred>
    struct mem_pred;

    template <typename mem_t, typename agg_t, mem_t agg_t::* mem_ptr, template <typename> class pred>
    struct mem_pred<mem_ptr, pred> {

        template <typename... arg_ts>
        constexpr auto operator()(const arg_ts&... args) const
            -> std::enable_if_t<is_same_v<agg_t, arg_ts...>, bool> {
            return pred<mem_t>{}(args.*mem_ptr...);
        }
    };

    template <auto mem_ptr>
    using mem_equal_to = mem_pred<mem_ptr, std::equal_to>;

    template <auto mem_ptr>
    using mem_not_equal_to = mem_pred<mem_ptr, std::not_equal_to>;

    template <auto mem_ptr>
    using mem_greater = mem_pred<mem_ptr, std::greater>;

    template <auto mem_ptr>
    using mem_less = mem_pred<mem_ptr, std::less>;

    template <auto mem_ptr>
    using mem_greater_equal = mem_pred<mem_ptr, std::greater_equal>;

    template <auto mem_ptr>
    using mem_less_equal = mem_pred<mem_ptr, std::less_equal>;
}

#endif
