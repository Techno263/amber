#pragma once

#include <cstddef>
#include <concepts>

namespace amber {

template<typename Allocator>
concept SimpleMemoryAllocator = requires(Allocator a, std::size_t n)
{
    { a.allocate(n, n) } -> std::same_as<void*>;
    { a.allocate(n) } -> std::same_as<void*>;
    { a.allocate<int>(n) } -> std::same_as<int*>;
    { a.allocate<int>() } -> std::same_as<int*>;
    { a.try_allocate(n, n) } noexcept -> std::same_as<void*>;
    { a.try_allocate(n) } noexcept -> std::same_as<void*>;
    { a.try_allocate<int>(n) } noexcept -> std::same_as<int*>;
    { a.try_allocate<int>() } noexcept -> std::same_as<int*>;
    { a.reset() } -> std::same_as<void>;
};

template<typename Allocator>
concept MemoryAllocator = requires(Allocator a, std::size_t n, void* vp, int* ip)
{
    { a.allocate(n, n) } -> std::same_as<void*>;
    { a.allocate(n) } -> std::same_as<void*>;
    { a.allocate<int>(n) } -> std::same_as<int*>;
    { a.allocate<int>() } -> std::same_as<int*>;
    { a.try_allocate(n, n) } noexcept -> std::same_as<void*>;
    { a.try_allocate(n) } noexcept -> std::same_as<void*>;
    { a.try_allocate<int>(n) } noexcept -> std::same_as<int*>;
    { a.try_allocate<int>() } noexcept -> std::same_as<int*>;
    { a.free(vp) } -> std::same_as<void>;
    { a.free<int>(ip) } -> std::same_as<void>;
    { a.try_free(vp) } noexcept -> std::same_as<bool>;
    { a.try_free(ip) } noexcept -> std::same_as<bool>;
    { a.reset() } noexcept -> std::same_as<void>;
}

}
