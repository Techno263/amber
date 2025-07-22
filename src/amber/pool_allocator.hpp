#pragma once

#include <amber/alloc_error.hpp>
#include <cstddef>
#include <expected>
#include <type_traits>

namespace amber {

class pool_allocator {
public:
    pool_allocator() = delete;

    pool_allocator(const pool_allocator&) = delete;

    pool_allocator(pool_allocator&& other) noexcept;

    static
    std::expected<pool_allocator, alloc_error> create(
        std::size_t buffer_alignment,
        std::size_t entry_alienment,
        std::size_t entry_size,
        std::size_t entry_count
    ) noexcept;

    static
    std::expected<pool_allocator, alloc_error> create(
        std::size_t entry_alignment,
        std::size_t entry_size,
        std::size_t entry_count
    ) noexcept;

    static
    std::expected<pool_allocator, alloc_error> create(
        std::size_t entry_size, std::size_t entry_count) noexcept;

    template<typename T>
    static
    std::expected<pool_allocator, alloc_error> create(std::size_t entry_count) noexcept;

    pool_allocator& operator=(const pool_allocator&) = delete;

    pool_allocator& operator=(pool_allocator&& other) noexcept;

    ~pool_allocator() noexcept;

    std::expected<void*, alloc_error> allocate() noexcept;

    template<typename T, typename... Args>
    requires std::is_nothrow_constructible_v<T, Args...>
    std::expected<T*, alloc_error> allocate(Args&&... args) noexcept;

    void free(void* ptr) noexcept;

    template<typename T>
    requires std::is_nothrow_destructible_v<T>
    void free(T* ptr) noexcept;

    std::size_t buffer_size() const noexcept;

    std::size_t entry_size() const noexcept;

    std::size_t entry_count() const noexcept;

private:
    pool_allocator(
        std::byte* buffer,
        std::byte* free_head,
        std::size_t buffer_size,
        std::size_t entry_size,
        std::size_t entry_count
    ) noexcept;

    std::byte* buffer_;
    std::byte* free_head_;
    std::size_t buffer_size_;
    std::size_t entry_size_;
    std::size_t entry_count_;
};

} // namespace amber

#include <amber/pool_allocator.inl>
