#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <string>

namespace amber {

std::expected<void*, std::string> aligned_alloc(std::size_t alignment, std::size_t size) noexcept;

void aligned_free(void* ptr) noexcept;

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr bool is_aligned(IntegerType alignment, IntegerType value) noexcept;

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType alignment, IntegerType value) noexcept;

} // namespace amber

#include <amber/util.inl>
