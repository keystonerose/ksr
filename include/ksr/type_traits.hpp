#ifndef KSR_TYPE_TRAITS_HPP
#define KSR_TYPE_TRAITS_HPP

#include "meta.hpp"
#include "meta_type_traits.hpp"

#include <type_traits>

namespace ksr {

    ///
    /// Determines whether a constructor declared equivalently to
    /// template <typename arg_t> t(arg_t&&)
    /// would match the copy or move constructor of t for a specific instantiation by \p arg_t. May
    /// be used with \c std::enable_if to ensure that universal constructors do are not instantiated
    /// when they may interfere with these special constructors.
    ///

    template <typename t, typename arg_t>
    struct matches_special_ctr : std::bool_constant<
        std::is_base_of_v<t, std::decay_t<arg_t>>> {};

    template <typename t, typename arg_t>
    inline constexpr auto matches_special_ctr_v = matches_special_ctr<t, arg_t>::value;

    namespace detail {

        template <typename t>
        constexpr auto underlying_type_ext(meta::type_tag<t>) {

            if constexpr (std::is_enum_v<t>) {
                return meta::type_tag<std::underlying_type_t<t>>{};
            } else {
                return meta::type_tag<std::remove_cv_t<t>>{};
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
    using underlying_type_ext = decltype(detail::underlying_type_ext(meta::type_tag<t>{}));

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
