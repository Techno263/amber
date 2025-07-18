#pragma once

#include <cstddef>

namespace amber {

class stack_allocator {
public:
    stack_allocator() = delete;

    stack_allocator(const stack_allocator&) = delete;

    stack_allocator(stack_allocator&& other) noexcept;

    explicit
    stack_allocator(std::size_t size);

    stack_allocator(std::size_t alignment, std::size_t size);

    stack_allocator& operator=(const stack_allocator&) = delete;

    stack_allocator& operator=(stack_allocator&& other) noexcept;

    ~stack_allocator() noexcept;

    void* allocate(std::size_t alignment, std::size_t size);

    void* allocate(std::size_t size);

    template<typename T>
    T* allocate();

    void* try_allocate(std::size_t alignment, std::size_t size) noexcept;

    void* try_allocate(std::size_t size) noexcept;

    template<typename T, typename... Args>
    requires std::is_nothrow_constructible_v<T, Args...>
    T* try_allocate() noexcept;

    void free(void* ptr);

    template<typename T>
    void free(T* ptr);

    bool try_free(void* ptr) noexcept;

    template<typename T>
    requires std::is_nothrow_destructible_v<T>
    bool try_free(T* ptr) noexcept;

private:
    std::byte* buffer;
    std::byte* previous_alloc;
    std::size_t buffer_size;
    std::size_t buffer_offset;

    friend auto allocate_helper(stack_allocator* a, std::size_t size) noexcept
        -> struct allocate_result;

    friend auto allocate_align_helper(stack_allocator* a, std::size_t alignment, std::size_t size) noexcept
        -> struct allocate_align_result;
};

}
