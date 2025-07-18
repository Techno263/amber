#pragma once

#include <concepts>
#include <cstdint>

namespace amber {

void* aligned_malloc_no_check(std::size_t alignment, std::size_t size);

void* aligned_malloc(std::size_t alignment, std::size_t size);

void* try_aligned_malloc_no_check(std::size_t alignment, std::size_t size) noexcept;

void* try_aligned_malloc(std::size_t alignment, std::size_t size) noexcept;

void aligned_free(void* ptr) noexcept;

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType is_aligned(IntegerType alignment, IntegerType value);

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType is_aligned_no_check(IntegerType alignment, IntegerType value) noexcept;

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType alignment, IntegerType value);

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward_no_check(IntegerType alignment, IntegerType value) noexcept;

} // namespace amber

#include <amber/util.inl>
