#ifndef KSR_META_TYPE_TRAITS_HPP
#define KSR_META_TYPE_TRAITS_HPP

#include <type_traits>

namespace ksr {

    /// Given a class template `value_func` with non-type (value) arguments only, determines whether
    /// `t` is an instantiation of `value_func`.
// TODO:HERE
    template <typename t, template <auto...> class value_func>
    struct in_value_domain : std::false_type {};

    template <template <auto...> class f, auto... vs>
    struct is_value_inst<f<vs...>, f> : std::true_type {};

    template <typename t, template <auto...> class f>
    inline constexpr auto is_value_inst_v = is_value_inst<t, f>::value;

    /// Given a class template `f` with type arguments only, determines whether `t` is an
    /// instantiation of `f`.

    template <typename t, template <typename...> class f>
    struct is_type_inst : std::false_type {};

    template <template <typename...> class f, typename... ts>
    struct is_type_inst<f<ts...>, f> : std::true_type {};

    template <typename t, template <typename...> class f>
    inline constexpr auto is_type_inst_v = is_type_inst<t, f>::value;

    /// Given a second-order class template `ff` -- that is,  with type arguments only, determines whether `t` is an
    /// instantiation of that template.

    template <typename t, template <template <typename...> class> class ff>
    struct is_func_inst : std::false_type {};

    template <template <template <typename...> class> class ff, typename... ts>
    struct is_func_inst<template_<ts...>, template_> : std::true_type {};

    template <typename t, template <template <typename...> class> class ff>
    inline constexpr auto is_func_inst_v = is_func_inst<t, ff>::value;
}

#endif
