#ifndef KSR_FLAG_PRODUCT_HPP
#define KSR_FLAG_PRODUCT_HPP

#include "type_util.hpp"

#include <tuple>

/// Given an scoped enumeration type `enum_t`, defines non-throwing, `constexpr` implementations of
/// the bitwise operators (that is, `&`, `|`, `^`, `~`, `&=`, `|=` and `^=`), each of which returns
/// another `enum_t` object.

#define KSR_DEFINE_BIT_OPERATIONS(enum_t) \
    [[maybe_unused]] constexpr auto operator&(const enum_t lhs, const enum_t rhs) noexcept -> enum_t { \
        return static_cast<enum_t>(under_cast(lhs) & under_cast(rhs)); \
    } \
    [[maybe_unused]] constexpr auto operator|(const enum_t lhs, const enum_t rhs) noexcept -> enum_t { \
        return static_cast<enum_t>(under_cast(lhs) | under_cast(rhs)); \
    } \
    [[maybe_unused]] constexpr auto operator^(const enum_t lhs, const enum_t rhs) noexcept -> enum_t { \
        return static_cast<enum_t>(under_cast(lhs) ^ under_cast(rhs)); \
    } \
    [[maybe_unused]] constexpr auto operator~(const enum_t value) noexcept -> enum_t { \
        return static_cast<enum_t>(~under_cast(value)); \
    } \
    [[maybe_unused]] constexpr auto operator&=(enum_t& lhs, const enum_t rhs) noexcept -> enum_t& { \
        return lhs = lhs & rhs; \
    } \
    [[maybe_unused]] constexpr auto operator|=(enum_t& lhs, const enum_t rhs) noexcept -> enum_t& { \
        return lhs = lhs | rhs; \
    } \
    [[maybe_unused]] constexpr auto operator^=(enum_t& lhs, const enum_t rhs) noexcept -> enum_t& { \
        return lhs = lhs ^ rhs; \
    }

template <typename... flag_ts>
class flag_product {
public:

    constexpr flag_product() = default;

    // TODO:HERE

    template <typename rhs_flag_t>
    friend constexpr auto operator|(const flag_product& lhs, const rhs_flag_t rhs) {

    }

    template <typename rhs_flag_ts...> // TODO:requires rhs_flag_ts to be a subset of flag_ts
    friend constexpr auto operator|(const flag_product& lhs, const flag_product<rhs_flag_ts...>& rhs) {

    }

private:

    std::tuple<flag_ts> m_flags;
};

#endif
