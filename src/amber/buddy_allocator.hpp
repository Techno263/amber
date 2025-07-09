#pragma once

#include <cstddef>

namespace amber {

class buddy_allocator {
public:
    buddy_allocator() = delete;

    buddy_allocator(const buddy_allocator&) = delete;

    buddy_allocator(buddy_allocator&& other) noexcept;

    buddy_allocator(std::size_t size);

    buddy_allocator(std::size_t size, std::size_t align);

    buddy_allocator& operator=(const buddy_allocator&) = delete;

    buddy_allocator& operator=(buddy_allocator&& other) noexcept;

    ~buddy_allocator() noexcept;

    void* allocate(std::size_t size);

    void* allocate(std::size_t size, std::size_t align);

    template<typename T>
    T* allocate()

private:

};

} // namespace amber
