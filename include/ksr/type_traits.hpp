#ifndef KSR_TYPE_TRAITS_HPP
#define KSR_TYPE_TRAITS_HPP

#include "meta.hpp"
#include <type_traits>

namespace ksr {

    namespace detail {

        template <typename t>
        constexpr auto underlying_type_ext(meta::tag<t>) {

            if constexpr (std::is_enum_v<t>) {
                return meta::tag<std::underlying_type_t<t>>{};
            } else {
                return meta::tag<std::remove_cv_t<t>>{};
            }
        }
    }

    ///
    /// An extension of \c std::underlying_type that may be applied to arbitrary types. If \p t is
    /// an enumeration type, provides the same behaviour as the standard library facility;
    /// otherwise, returns \p t itself, with any CV qualifiers removed (for consistency with former
    /// case).
    ///

    template <typename t>
    using underlying_type_ext = decltype(detail::underlying_type_ext(meta::tag<t>{}));

    template <typename t>
    using underlying_type_ext_t = typename underlying_type_ext<t>::type;

    ///
    /// Determines whether the type \p t is arithmetic once transformed to any potentially
    /// underlying type as per \ref underlying_type_ext. Crucially, this includes enumeration types
    /// as well as those that would normally be identified by \c std::is_arithmetic.
    ///

    template <typename t>
    struct is_numeric : std::bool_constant<
        std::is_arithmetic_v<underlying_type_ext_t<t>>> {};

    template <typename t>
    inline constexpr auto is_numeric_v = is_numeric<t>::value;
}

#endif
