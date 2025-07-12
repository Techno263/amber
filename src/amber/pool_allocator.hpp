#pragma once

#include <cstddef>
#include <type_traits>

namespace amber {

class pool_allocator {
public:
    pool_allocator() = delete;

    pool_allocator(const pool_allocator&) = delete;

    pool_allocator(pool_allocator&& other) noexcept;

    pool_allocator(std::size_t entry_size, std::size_t entry_count);

    pool_allocator(std::size_t entry_size, std::size_t entry_count, std::size_t entry_align);

    pool_allocator& operator=(const pool_allocator&) = delete;

    pool_allocator& operator=(pool_allocator&& other) noexcept;

    ~pool_allocator() noexcept;

    void* allocate();

    template<typename T, typename... Args>
    T* allocate(Args&&... args);

    void* try_allocate() noexcept;

    template<typename T, typename... Args>
    requires std::is_nothrow_constructible_v<T, Args...>
    T* try_allocate(Args&&... args) noexcept;

    void free(void* ptr);

    template<typename T>
    void free(T* ptr);

    bool try_free(void* ptr) noexcept;

    template<typename T>
    requires std::is_nothrow_destructible_v<T>
    bool try_free(T* ptr) noexcept;

private:
    void init_buffer() noexcept;

    std::byte* buffer;
    std::byte* free_head;
    std::size_t buffer_size;
    std::size_t entry_size;
    std::size_t entry_count;
};

} // namespace amber

#include <amber/pool_allocator.inl>
