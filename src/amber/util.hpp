#pragma once

#include <amber/alloc_error.hpp>
#include <concepts>
#include <cstdint>
#include <expected>

namespace amber {

std::expected<void*, alloc_error> aligned_alloc(std::size_t alignment, std::size_t size) noexcept;

void aligned_free(void* ptr) noexcept;

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr bool is_aligned(IntegerType alignment, IntegerType value) noexcept;

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType alignment, IntegerType value) noexcept;

} // namespace amber

#include <amber/util.inl>
