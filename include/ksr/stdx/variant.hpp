#ifndef KSR_STDX_VARIANT_HPP
#define KSR_STDX_VARIANT_HPP

#include "../meta.hpp"

#include <variant>

namespace ksr { namespace stdx {

    // These adapter aliases for `std::variant` instantiate that template with the `extra_ts`
    // parameters before those of `type_seq_t`; this allows `std::monostate` to be used as the first
    // type in the variant.

    template <typename type_seq_t, typename... extra_ts>
    using variant = meta::apply_types_t<decltype(meta::concat(meta::type_seq<extra_ts...>{}, type_seq_t{})), std::variant>;

    template <template <typename> class op, typename type_seq_t, typename... extra_ts>
    using type_domain_variant = variant<meta::transform_types_t<type_seq_t, op>, extra_ts...>;

    template <template <auto> class op, typename value_seq_t, typename... extra_ts>
    using value_domain_variant = variant<meta::transform_values_t<value_seq_t, op>, extra_ts...>;
}}

#endif
