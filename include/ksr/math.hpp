#ifndef KSR_MATH_HPP
#define KSR_MATH_HPP

#include "error.hpp"
#include "type_util.hpp"
#include <cmath>

namespace ksr {

    template <typename num_t, typename = std::enable_if_t<std::is_arithmetic_v<num_t>>>
    auto int_percentage(const num_t num, const num_t denom) -> int {

        KSR_ASSERT(denom != num_t{});
        const auto result = std::lround(100.0 * static_cast<double>(num) / static_cast<double>(denom));
        return narrow_cast<int>(result);
    }
}

#endif
