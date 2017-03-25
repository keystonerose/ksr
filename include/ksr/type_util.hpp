#ifndef KSR_TYPE_UTIL_HPP
#define KSR_TYPE_UTIL_HPP

#include "error.hpp"
#include "type_traits.hpp"

#include <type_traits>

namespace ksr {

    namespace detail {

        ///
        /// Determines whether a specified arithmetic or enumeration value is negative; that is,
        /// less than a zero-initialized value of the same type.
        ///

        template <typename t>
        constexpr auto is_negative(const t value) -> std::enable_if_t<is_numeric_v<t>, bool> {
            using underlying_t = underlying_type_ext_t<t>;
            return static_cast<underlying_type_t>(value) < underlying_t{};
        }
    }

    ///
    /// Uses \c static_cast to perform a narrowing conversion of the value \p input to a
    /// corresponding value of type \p output_t. \p input_t and \p output_t must be types presenting
    /// a potentially-narrowing conversion as per \ref ksr::can_narrow. For release builds, the
    /// behaviour of this function is identical to that of \c static_cast and it generates no
    /// machine code; in debug builds, it raises an error via \c KSR_ASSERT when \p input is not
    /// representable by \p output_t.
    ///
    /// This is an adaptation of the functionality provided by <tt>gsl::narrow()</tt>, but extends
    /// that functionality to also cover enumeration types and, in providing identical behaviour to
    /// \c static_cast for release builds, facilitates more extensive use.
    ///

    template <typename output_t, typename input_t>
    constexpr auto narrow_cast(const input_t input)
        -> std::enable_if_t<is_numeric_v<output_t> && is_numeric_v<input_t>, output_t> {

        const auto output = static_cast<output_t>(input);
        KSR_ASSERT(static_cast<input_t>(output) == input);
        KSR_ASSERT(detail::is_negative(output) == detail::is_negative(input));

        return output;
    }
}

#endif
