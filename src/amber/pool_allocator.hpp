#pragma once

#include <amber/concept.hpp>
#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <type_traits>

namespace amber {

namespace internal {

struct pool_entry {
public:
    pool_entry(std::byte* next) noexcept;

    std::byte* next;
};

} // namespace amber::internal

class pool_allocator {
public:
    pool_allocator() = delete;

    pool_allocator(const pool_allocator&) = delete;

    pool_allocator(pool_allocator&& other) noexcept;

    pool_allocator& operator=(const pool_allocator&) = delete;

    pool_allocator& operator=(pool_allocator&& other) noexcept;

    ~pool_allocator() noexcept;

    template<Buffer B>
    static
    std::expected<pool_allocator, std::string> create(
        B& buffer, std::size_t entry_size) noexcept;

    std::expected<void*, std::string> allocate() noexcept;

    template<typename T, typename... Args>
    requires std::is_nothrow_constructible_v<T, Args...>
    std::expected<T*, std::string> allocate(Args&&... args) noexcept;

    void free(void* ptr) noexcept;

    template<typename T>
    requires std::is_nothrow_destructible_v<T>
    void free(T* ptr) noexcept;

    std::size_t buffer_size() const noexcept;

    std::size_t entry_size() const noexcept;

    std::size_t entry_count() const noexcept;

    std::size_t entry_allocate_count() const noexcept;

    std::size_t entry_free_count() const noexcept;

private:
    pool_allocator(
        std::span<std::byte> buffer,
        std::byte* free_head,
        std::size_t entry_size,
        std::size_t entry_count,
        std::size_t entry_allocate_count
    ) noexcept;

    std::span<std::byte> buffer_;
    std::byte* free_head_;
    std::size_t entry_size_;
    std::size_t entry_count_;
    std::size_t entry_allocate_count_;
};

} // namespace amber

#include <amber/pool_allocator.inl>
