#pragma once

#include <concepts>

namespace amber {

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType num, IntegerType align);

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward_no_check(IntegerType num, IntegerType align) noexcept;

} // namespace amber

#include <amber/util.inl>
