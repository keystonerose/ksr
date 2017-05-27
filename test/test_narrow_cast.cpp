#include "ksr/type_util.hpp"

#include "catch/catch.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

using namespace ksr;

namespace {

    template <typename T>
    constexpr auto min = std::numeric_limits<T>::min();

    template <typename T>
    constexpr auto max = std::numeric_limits<T>::max();

    constexpr auto min_u8 = min<std::uint8_t>;
    constexpr auto max_u8 = max<std::uint8_t>;
    constexpr auto min_i8 = min<std::int8_t>;
    constexpr auto max_i8 = max<std::int8_t>;

    enum class u8_enum : std::uint8_t { min = min_u8, max = max_u8 };
    enum class i8_enum : std::int8_t  { min = min_i8, max = max_i8 };

    constexpr auto min_u32 = min<std::uint32_t>;
    constexpr auto max_u32 = max<std::uint32_t>;
    constexpr auto min_i32 = min<std::int32_t>;
    constexpr auto max_i32 = max<std::int32_t>;

    enum class u32_enum : std::uint32_t { min = min_u32, max = max_u32 };
    enum class i32_enum : std::int32_t  { min = min_i32, max = max_i32 };

    template <typename output_t, typename input_t>
    auto can_cast(const input_t input) -> bool {

        try {
            const auto output = narrow_cast<output_t>(input);
            return output == static_cast<output_t>(input);
        } catch (const logic_error&) {
            return false;
        }
    }
}

TEST_CASE("enum_to_integral", "[type_util][narrow_cast]") {

    CHECK(can_cast<std::int8_t>(i8_enum::min));
    CHECK(can_cast<std::int8_t>(i8_enum::max));
    CHECK(can_cast<std::int8_t>(u8_enum::min));
    CHECK(can_cast<std::int8_t>(u32_enum::min));

    CHECK(can_cast<std::uint8_t>(i8_enum::max));
    CHECK(can_cast<std::uint8_t>(u8_enum::min));
    CHECK(can_cast<std::uint8_t>(u8_enum::max));
    CHECK(can_cast<std::uint8_t>(u32_enum::min));

    CHECK(can_cast<std::int32_t>(i32_enum::min));
    CHECK(can_cast<std::int32_t>(i32_enum::max));
    CHECK(can_cast<std::int32_t>(u32_enum::min));

    CHECK(can_cast<std::uint32_t>(u32_enum::min));
    CHECK(can_cast<std::uint32_t>(u32_enum::max));
    CHECK(can_cast<std::uint32_t>(i32_enum::max));

    CHECK_FALSE(can_cast<std::int8_t>(u8_enum::max));
    CHECK_FALSE(can_cast<std::int8_t>(i32_enum::min));
    CHECK_FALSE(can_cast<std::int8_t>(i32_enum::max));
    CHECK_FALSE(can_cast<std::int8_t>(u32_enum::max));

    CHECK_FALSE(can_cast<std::uint8_t>(i8_enum::min));
    CHECK_FALSE(can_cast<std::uint8_t>(i32_enum::min));
    CHECK_FALSE(can_cast<std::uint8_t>(i32_enum::max));
    CHECK_FALSE(can_cast<std::uint8_t>(u32_enum::max));

    CHECK_FALSE(can_cast<std::int32_t>(u32_enum::max));
    CHECK_FALSE(can_cast<std::uint32_t>(i32_enum::min));
}

TEST_CASE("floating_point", "[type_util][narrow_cast]") {

    static constexpr auto double_inf = std::numeric_limits<double>::infinity();
    static constexpr auto min_float = min<float>;
    static constexpr auto max_float = max<float>;
    static constexpr auto min_double = min<double>;
    static constexpr auto max_double = max<double>;

    CHECK(can_cast<float>(min_float));
    CHECK(can_cast<float>(max_float));
    CHECK(can_cast<float>(double{min_float}));
    CHECK(can_cast<float>(double{max_float}));

    CHECK(can_cast<std::int32_t>(min_i8));
    CHECK(can_cast<std::int32_t>(max_i8));
    CHECK(can_cast<std::int32_t>(min_i32));
    CHECK(can_cast<std::int32_t>(max_i32));

    CHECK_FALSE(can_cast<float>(min_double));
    CHECK_FALSE(can_cast<float>(std::nextafter(double{min_float}, -double_inf)));
    CHECK_FALSE(can_cast<float>(std::nextafter(double{max_float}, double_inf)));
    CHECK_FALSE(can_cast<float>(max_double));
}

TEST_CASE("integral_to_enum", "[type_util][narrow_cast]") {

    CHECK(can_cast<i8_enum>(min_i8));
    CHECK(can_cast<i8_enum>(max_i8));
    CHECK(can_cast<i8_enum>(min_u8));
    CHECK(can_cast<i8_enum>(min_u32));

    CHECK(can_cast<u8_enum>(max_i8));
    CHECK(can_cast<u8_enum>(min_u8));
    CHECK(can_cast<u8_enum>(max_u8));
    CHECK(can_cast<u8_enum>(min_u32));

    CHECK(can_cast<i32_enum>(min_i32));
    CHECK(can_cast<i32_enum>(max_i32));
    CHECK(can_cast<i32_enum>(min_u32));

    CHECK(can_cast<u32_enum>(min_u32));
    CHECK(can_cast<u32_enum>(max_u32));
    CHECK(can_cast<u32_enum>(max_i32));

    CHECK_FALSE(can_cast<i8_enum>(max_u8));
    CHECK_FALSE(can_cast<i8_enum>(min_i32));
    CHECK_FALSE(can_cast<i8_enum>(max_i32));
    CHECK_FALSE(can_cast<i8_enum>(max_u32));

    CHECK_FALSE(can_cast<u8_enum>(min_i8));
    CHECK_FALSE(can_cast<u8_enum>(min_i32));
    CHECK_FALSE(can_cast<u8_enum>(max_i32));
    CHECK_FALSE(can_cast<u8_enum>(max_u32));

    CHECK_FALSE(can_cast<i32_enum>(max_u32));
    CHECK_FALSE(can_cast<u32_enum>(min_i32));
}

TEST_CASE("mixed_signedness", "[type_util][narrow_cast]") {

    CHECK(can_cast<std::uint8_t>(std::int8_t{min_u8}));
    CHECK(can_cast<std::uint8_t>(max_i8));
    CHECK(can_cast<std::uint8_t>(std::int32_t{min_u8}));
    CHECK(can_cast<std::uint8_t>(std::int32_t{max_u8}));

    CHECK(can_cast<std::int8_t>(min_u8));
    CHECK(can_cast<std::int8_t>(std::uint8_t{max_i8}));
    CHECK(can_cast<std::int8_t>(min_u32));
    CHECK(can_cast<std::int8_t>(std::uint32_t{max_i8}));

    CHECK(can_cast<std::uint32_t>(std::int32_t{min_u32}));
    CHECK(can_cast<std::uint32_t>(max_i32));

    CHECK(can_cast<std::int32_t>(min_u32));
    CHECK(can_cast<std::int32_t>(std::uint32_t{max_i32}));

    CHECK_FALSE(can_cast<std::uint8_t>(min_i8));
    CHECK_FALSE(can_cast<std::uint8_t>(std::int8_t{signed{min_u8} - 1}));
    CHECK_FALSE(can_cast<std::uint8_t>(min_i32));
    CHECK_FALSE(can_cast<std::uint8_t>(std::int32_t{signed{min_u8} - 1}));
    CHECK_FALSE(can_cast<std::uint8_t>(std::int32_t{signed{max_u8} + 1}));
    CHECK_FALSE(can_cast<std::uint8_t>(max_i32));

    CHECK_FALSE(can_cast<std::int8_t>(std::uint8_t{max_i8 + 1}));
    CHECK_FALSE(can_cast<std::int8_t>(max_u8));
    CHECK_FALSE(can_cast<std::int8_t>(std::uint32_t{max_i8 + 1}));
    CHECK_FALSE(can_cast<std::int8_t>(max_u32));

    CHECK_FALSE(can_cast<std::uint32_t>(min_i32));
    CHECK_FALSE(can_cast<std::uint32_t>(std::int32_t{signed{min_u32} - 1}));

    CHECK_FALSE(can_cast<std::int32_t>(std::uint32_t{unsigned{max_i32} + 1}));
    CHECK_FALSE(can_cast<std::int32_t>(max_u32));
}

TEST_CASE("signed", "[type_util][narrow_cast]") {

    CHECK(can_cast<std::int8_t>(min_i8));
    CHECK(can_cast<std::int8_t>(max_i8));
    CHECK(can_cast<std::int8_t>(std::int32_t{min_i8}));
    CHECK(can_cast<std::int8_t>(std::int32_t{max_i8}));

    CHECK(can_cast<std::int32_t>(min_i8));
    CHECK(can_cast<std::int32_t>(max_i8));
    CHECK(can_cast<std::int32_t>(min_i32));
    CHECK(can_cast<std::int32_t>(max_i32));

    CHECK_FALSE(can_cast<std::int8_t>(min_i32));
    CHECK_FALSE(can_cast<std::int8_t>(std::int32_t{min_i8 - 1}));
    CHECK_FALSE(can_cast<std::int8_t>(std::int32_t{max_i8 + 1}));
    CHECK_FALSE(can_cast<std::int8_t>(max_i32));
}

TEST_CASE("unsigned", "[type_util][narrow_cast]") {

    CHECK(can_cast<std::uint8_t>(min_u8));
    CHECK(can_cast<std::uint8_t>(max_u8));
    CHECK(can_cast<std::uint8_t>(std::uint32_t{min_u8}));
    CHECK(can_cast<std::uint8_t>(std::uint32_t{max_u8}));

    CHECK(can_cast<std::uint32_t>(min_u8));
    CHECK(can_cast<std::uint32_t>(max_u8));
    CHECK(can_cast<std::uint32_t>(min_u32));
    CHECK(can_cast<std::uint32_t>(max_u32));

    CHECK_FALSE(can_cast<std::uint8_t>(std::uint32_t{int{max_u8} + 1}));
    CHECK_FALSE(can_cast<std::uint8_t>(max_u32));
}
