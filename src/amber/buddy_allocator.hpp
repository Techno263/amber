#pragma once

#include <amber/alloc_error.hpp>
#include <cstddef>
#include <expected>
#include <type_traits>

namespace amber {

class buddy_allocator {
public:
    buddy_allocator() = delete;

    buddy_allocator(const buddy_allocator&) = delete;

    buddy_allocator(buddy_allocator&& other) noexcept;

    static
    std::expected<buddy_allocator, alloc_error> create(
        std::size_t alignment, std::size_t size) noexcept;

    static
    std::expected<buddy_allocator, alloc_error> create(
        std::size_t size) noexcept;

    buddy_allocator& operator=(const buddy_allocator&) = delete;

    buddy_allocator& operator=(buddy_allocator&& other) noexcept;

    ~buddy_allocator() noexcept;

    std::expected<void*, alloc_error> allocate(
        std::size_t alignment, std::size_t size) noexcept;

    std::expected<void*, alloc_error> allocate(std::size_t size) noexcept;

    template<typename T, typename... Args>
    requires std::is_nothrow_constructible_v<T, Args...>
    std::expected<T*, alloc_error> allocate(Args&&... args) noexcept;

    void free(void* ptr) noexcept;

    template<typename T>
    requires std::is_nothrow_destructible_v<T>
    void free(T* ptr) noexcept;

private:
    buddy_allocator(
        std::byte* buffer,
        std::size_t buffer_size,
    ) noexcept;

    std::byte* buffer_;
    std::size_t buffer_size_;
};

} // namespace amber

#include <amber/buddy_allocator.inl>
