#pragma once

#include <amber/concept.hpp>
#include <cstddef>
#include <expected>
#include <string>
#include <type_traits>

namespace amber {

class linear_allocator {
public:
    linear_allocator() = delete;

    linear_allocator(const linear_allocator&) = delete;

    linear_allocator(linear_allocator&& other) noexcept;

    linear_allocator& operator=(const linear_allocator&) = delete;

    linear_allocator& operator=(linear_allocator&& other) noexcept;

    ~linear_allocator() noexcept;

    template<Buffer B>
    static
    std::expected<linear_allocator, std::string> create(B& buffer) noexcept;

    std::expected<void*, std::string> allocate(
        std::size_t alignment, std::size_t size) noexcept;

    std::expected<void*, std::string> allocate(std::size_t size) noexcept;

    template<typename T, typename... Args>
    requires std::is_trivially_destructible_v<T>
    std::expected<T*, std::string> allocate(Args&&... args) noexcept;

    void reset() noexcept;

    std::size_t buffer_size() const noexcept;

    std::size_t buffer_offset() const noexcept;

private:
    linear_allocator(
        std::byte* buffer,
        std::size_t buffer_size,
        std::size_t buffer_offset
    ) noexcept;

    std::byte* buffer_;
    std::size_t buffer_size_;
    std::size_t buffer_offset_;
};

} // namespace amber

#include <amber/linear_allocator.inl>
