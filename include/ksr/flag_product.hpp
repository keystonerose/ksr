#ifndef KSR_FLAG_PRODUCT_HPP
#define KSR_FLAG_PRODUCT_HPP

#include "type_util.hpp"

/// Given an scoped enumeration type `flags_t`, defines non-throwing, `constexpr` implementations of
/// the bitwise operators (that is, `&`, `|`, `^`, `~`, `&=`, `|=` and `^=`), each of which returns
/// another `flags_t` object.

#define KSR_DEFINE_FLAGS(flags_t) \
    [[maybe_unused]] constexpr auto operator&(const flags_t lhs, const flags_t rhs) noexcept -> flags_t { \
        return static_cast<flags_t>(under_cast(lhs) & under_cast(rhs)); \
    } \
    [[maybe_unused]] constexpr auto operator|(const flags_t lhs, const flags_t rhs) noexcept -> flags_t { \
        return static_cast<flags_t>(under_cast(lhs) | under_cast(rhs)); \
    } \
    [[maybe_unused]] constexpr auto operator^(const flags_t lhs, const flags_t rhs) noexcept -> flags_t { \
        return static_cast<flags_t>(under_cast(lhs) ^ under_cast(rhs)); \
    } \
    [[maybe_unused]] constexpr auto operator~(const flags_t value) noexcept -> flags_t { \
        return static_cast<flags_t>(~under_cast(value)); \
    } \
    [[maybe_unused]] constexpr auto operator&=(flags_t& lhs, const flags_t rhs) noexcept -> flags_t& { \
        return lhs = lhs & rhs; \
    } \
    [[maybe_unused]] constexpr auto operator|=(flags_t& lhs, const flags_t rhs) noexcept -> flags_t& { \
        return lhs = lhs | rhs; \
    } \
    [[maybe_unused]] constexpr auto operator^=(flags_t& lhs, const flags_t rhs) noexcept -> flags_t& { \
        return lhs = lhs ^ rhs; \
    }

#endif
