#ifndef KSR_META_TYPE_TRAITS_HPP
#define KSR_META_TYPE_TRAITS_HPP

#include <type_traits>

namespace ksr {

    ///
    /// Given a class template \p template_ with type arguments only, determines whether \p t is an
    /// instantiation of that template.
    ///

    template <typename t, template <typename...> class template_>
    struct is_type_inst : std::false_type {};

    template <template <typename...> class template_, typename... ts>
    struct is_type_inst<template_<ts...>, template_> : std::true_type {};

    template <typename t, template <typename...> class template_>
    inline constexpr auto is_type_inst_v = is_type_inst<t, template_>::value;

    ///
    /// Given a class template \p template_ with non-type (value) arguments only, determines whether
    /// \p t is an instantiation of that template.
    ///

    template <typename t, template <auto...> class template_>
    struct is_value_inst : std::false_type {};

    template <template <auto...> class template_, auto... vs>
    struct is_value_inst<template_<vs...>, template_> : std::true_type {};

    template <typename t, template <auto...> class template_>
    inline constexpr auto is_value_inst_v = is_value_inst<t, template_>::value;
}

#endif
