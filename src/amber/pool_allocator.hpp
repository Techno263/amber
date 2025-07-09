#pragma once

#include <cstddef>

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

    template<typename T>
    T* allocate();

    template<typename T, typename... Args>
    T* allocate_with_construct(Args&&... args);

    void* try_allocate() noexcept;

    template<typename T>
    T* try_allocate() noexcept;

    void free(void* ptr);

    template<typename T>
    void free(T* ptr);

    template<typename T>
    void free_with_destruct(T* ptr);

    bool try_free(void* ptr) noexcept;

    template<typename T>
    bool try_free(T* ptr) noexcept;

    void reset() noexcept;

private:
    std::void* buffer;
    std::void* free_head;
    std::size_t buffer_size;
    std::size_t entry_size;
    std::size_t entry_count;
};

} // namespace amber

#include <amber/pool_allocator.inl>
