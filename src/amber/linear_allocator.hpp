#pragma once

#include <cstddef>
#include <type_traits>

namespace amber {

class linear_allocator {
public:
    linear_allocator() = delete;

    linear_allocator(const linear_allocator&) = delete;

    linear_allocator(linear_allocator&&) noexcept;

    explicit
    linear_allocator(std::size_t size);

    linear_allocator(std::size_t alignment, std::size_t size);

    linear_allocator& operator=(const linear_allocator&) = delete;

    linear_allocator& operator=(linear_allocator&&) noexcept;

    ~linear_allocator() noexcept;

    void* allocate(std::size_t alignment, std::size_t size);

    void* allocate(std::size_t size);

    template<typename T, typename... Args>
    requires std::is_trivially_destructible_v<T>
    T* allocate(Args&&... args);

    void* try_allocate(std::size_t alignment, std::size_t size) noexcept;

    void* try_allocate(std::size_t size) noexcept;

    template<typename T, typename... Args>
    requires std::is_trivially_destructible_v<T> && std::is_nothrow_constructible_v<T, Args...>
    T* try_allocate(Args&&... args) noexcept;

    void reset() noexcept;

    std::size_t size() const;

    std::size_t offset() const;

private:
    std::byte* buffer;
    std::size_t buffer_size;
    std::size_t buffer_offset;
};

} // namespace amber

#include <amber/linear_allocator.inl>
