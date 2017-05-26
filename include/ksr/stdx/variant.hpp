#ifndef KSR_STDX_VARIANT_HPP
#define KSR_STDX_VARIANT_HPP

#include "../meta.hpp"

#include <variant>

namespace ksr { namespace stdx {

    // These adapter aliases for `std::variant` instantiate that template with the `extra_ts`
    // parameters before those of `type_seq_t`; this allows `std::monostate` to be used as the first
    // type in the variant.

    template <typename type_seq_t, typename... extra_ts>
    using variant = decltype(meta::apply_to<std::variant>(meta::concat(meta::type_seq<extra_ts...>{}, type_seq_t{})));

    template <template <typename> class op, typename seq_t, typename... extra_ts>
    using type_domain_variant = variant<decltype(meta::transform_by<op>(seq_t{})), extra_ts...>;

    template <template <auto> class op, typename seq_t, typename... extra_ts>
    using value_domain_variant = variant<decltype(meta::transform_by<op>(seq_t{})), extra_ts...>;
}}

#endif
