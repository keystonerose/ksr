#ifndef KSR_STDX_ARRAY_HPP
#define KSR_STDX_ARRAY_HPP

#include <array>
#include <type_traits>
#include <utility>

namespace ksr { namespace stdx {

    /// Creates a `std::array` containing the given arguments, inferring the number of elements and,
    /// optionally, the value type of the array. If no argument is given for `value_t` or `value_t`
    /// is `void`, the value type of the returned array is the common type of the arguments (in the
    /// sense of `std::common_type`); otherwise, it is `value_t`. Will ultimately be deprecated once
    /// the C++17 deduction guides for `std::array` are available.

    template <typename value_t = void, typename... arg_ts>
    constexpr auto make_array(arg_ts&&... args) {

        using ext_value_t = std::conditional_t<
            std::is_same_v<value_t, void>, std::common_type_t<arg_ts...>, value_t>;

        return std::array<ext_value_t, sizeof...(arg_ts)>{std::forward<arg_ts>(args)...};
    }
}}

#endif
