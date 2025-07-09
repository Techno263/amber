#pragma once

#include <type_trait>

namespace amber {

template<typename IntegerType>
requires std::is_integral_v<IntegerType>
constexpr bool is_power_of_two(IntegerType n) noexcept;

template<typename IntegerType>
requires std::is_integral_v<IntegerType>
constexpr IntegerType align_forward(IntegerType num, IntegerType align);

template<typename IntegerType>
requires std::is_integral_v<IntegerType>
constexpr IntegerType align_forward_no_check(IntegerType num, IntegerType align) noexcept;

} // namespace amber

#include <amber/util.inl>
