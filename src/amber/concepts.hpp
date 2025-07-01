#pragma once

#include <cstddef>
#include <concepts>

namespace amber {

template<typename Allocator, typename T, std::size_t n>
concept LinearMemoryAllocator = requires(Allocator a)
{
    { a.allocate(n, n) } -> std::same_as<void*>;
    { a.allocate(n) } -> std::same_as<void*>;
    { a.allocate<unsigned char>(n) } -> std::same_as<unsigned char*>;
    { a.allocate<unsigned char>() } -> std::same_as<unsigned char*>;
    { a.try_allocate(n, n) } noexcept -> std::same_as<void*>;
    { a.try_allocate(n) } noexcept -> std::same_as<void*>;
    { a.try_allocate<unsigned char>(n) } noexcept -> std::same_as<unsigned char*>;
    { a.try_allocate<unsigned char>() } noexcept -> std::same_as<unsigned char*>;
    { a.reset() } -> std::same_as<void>;
};

}
