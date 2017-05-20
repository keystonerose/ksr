#ifndef KSR_STDX_VARIANT_HPP
#define KSR_STDX_VARIANT_HPP

#include "../meta.hpp"

#include <variant>

namespace ksr { namespace stdx {

    template <typename type_seq>
    using variant_of = meta::apply_types_t<type_seq, std::variant>;

    template <typename type_seq>
    using opt_variant_of = variant_of<decltype(meta::push_front<std::monostate>(type_seq{}))>;
}}

#endif
