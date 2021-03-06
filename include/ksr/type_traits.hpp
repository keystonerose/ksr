#ifndef KSR_TYPE_TRAITS_HPP
#define KSR_TYPE_TRAITS_HPP

#include "meta.hpp"
#include "meta_type_traits.hpp"

#include <type_traits>

namespace ksr {

    /// Determines whether a constructor declared equivalently to
    /// `template <typename arg_t> t(arg_t&&)` would match the copy or move constructor of `t` for a
    /// specific instantiation by `arg_t`. May be used with `std::enable_if` to ensure that
    /// universal constructors are not instantiated when they may interfere with these special
    /// constructors.

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

    /// An extension of `std::underlying_type` that may be applied to arbitrary types. If `t` is an
    /// enumeration type, provides the same behaviour as the standard library facility; otherwise,
    /// returns `t` itself, with any CV qualifiers removed (for consistency with former case).

    template <typename t>
    using underlying_type_ext = decltype(detail::underlying_type_ext(meta::type_tag<t>{}));

    template <typename t>
    using underlying_type_ext_t = typename underlying_type_ext<t>::type;

    /// Determines whether the type `t` is arithmetic once transformed to any potentially underlying
    /// type as per `underlying_type_ext`. Crucially, this includes enumeration types
    /// as well as those that would normally be identified by `std::is_arithmetic`.

    template <typename t>
    struct is_numeric : std::bool_constant<
        std::is_arithmetic_v<underlying_type_ext_t<t>>> {};

    template <typename t>
    inline constexpr auto is_numeric_v = is_numeric<t>::value;

    /// Determines whether every type in `ts` is the same as `ref_t`, in the sense of
    /// `std::is_same` (and may therefore be considered a variadic extension of that standard
    /// library trait).

    template <typename ref_t, typename... ts>
    using is_same = std::conjunction<std::is_same<ref_t, ts>...>;

    template <typename ref_t, typename... ts>
    inline constexpr auto is_same_v = is_same<ref_t, ts...>::value;

    /// Gets the type of member pointed to by `mem_ptr`, where `mem_ptr` is a pointer to a member
    /// of any class type to the type of the member that it points to.

    template <auto mem_ptr>
    struct mem_type;

    template <typename mem_t, typename t, mem_t t::* mem_ptr>
    struct mem_type<mem_ptr> {
        using type = mem_t;
    };

    template <auto mem_ptr>
    using mem_type_t = typename mem_type<mem_ptr>::type;
}

#endif
