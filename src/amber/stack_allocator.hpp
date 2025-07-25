#pragma once

#include <amber/alloc_error.hpp>
#include <cstddef>
#include <expected>
#include <type_traits>

namespace amber {

class stack_allocator {
public:
    stack_allocator() = delete;

    stack_allocator(const stack_allocator&) = delete;

    stack_allocator(stack_allocator&& other) noexcept;

    static
    std::expected<stack_allocator, alloc_error> create(
        std::size_t alignment,
        std::size_t size
    ) noexcept;

    static
    std::expected<stack_allocator, alloc_error> create(std::size_t size) noexcept;

    stack_allocator& operator=(const stack_allocator&) = delete;

    stack_allocator& operator=(stack_allocator&& other) noexcept;

    ~stack_allocator() noexcept;

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

    std::size_t buffer_size() const noexcept;

    std::size_t buffer_offset() const noexcept;

private:
    stack_allocator(
        std::byte* buffer,
        std::size_t buffer_size,
        std::size_t buffer_offset
    ) noexcept;

    std::byte* buffer_;
    std::size_t buffer_size_;
    std::size_t buffer_offset_;
};

} // namespace amber

#include <amber/stack_allocator.inl>
