#pragma once

#include <cstddef>
#include <amber/concepts.hpp>

namespace amber {

class stack_allocator {
public:
    stack_allocator() = delete;

    stack_allocator(const stack_allocator&) = delete;

    stack_allocator(stack_allocator&&) noexcept;

    explicit
    stack_allocator(std::size_t size);

    stack_allocator(std::size_t size, std::size_t align);

    stack_allocator& operator=(const stack_allocator&) = delete;

    stack_allocator& operator=(stack_allocator&&) noexcept;

    ~stack_allocator() noexcept;

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

    void free(void* ptr);

    template<typename T>
    void free(T* ptr);

    bool try_free(void* ptr) noexcept;

    template<typename T>
    bool try_free(T* ptr) noexcept;

    void reset() noexcept;

private:
    std::byte* buffer;
    std::size_t buffer_size;
    std::size_t buffer_offset;
};

static_assert(MemoryAllocator<stack_allocator>):

}
