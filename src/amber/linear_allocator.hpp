#pragma once

#include <cstddef>
#include <amber/concepts.hpp>

namespace amber {

class linear_allocator {
public:
    linear_allocator() = delete;

    linear_allocator(const linear_allocator&) = delete;

    linear_allocator(linear_allocator&&) noexcept;

    explicit
    linear_allocator(std::size_t size);

    linear_allocator(std::size_t size, std::size_t align);

    linear_allocator& operator=(const linear_allocator&) = delete;

    linear_allocator& operator=(linear_allocator&&) noexcept;

    ~linear_allocator() noexcept;

    void* allocate(std::size_t size, std::size_t align);

    void* allocate(std::size_t size);

    template<typename T>
    T* allocate(std::size_t count);

    template<typename T>
    T* allocate();

    void* try_allocate(std::size_t size, std::size_t align) noexcept;

    void* try_allocate(std::size_t size) noexcept;

    template<typename T>
    T* try_allocate(std::size_t count) noexcept;

    template<typename T>
    T* try_allocate() noexcept;

    void reset() noexcept;

private:
    std::byte* buffer;
    std::size_t buffer_size;
    std::size_t buffer_offset;
};

static_assert(LinearMemoryAllocator<linear_allocator>);

}

#include <amber/linear_allocator.inl>
