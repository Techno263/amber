#include <amber/linear_allocator.hpp>
#include <cstdlib>
#include <new>
#include <utility>
#include <amber/except.hpp>
#include <amber/util.hpp>

namespace amber {

linear_allocator::linear_allocator(linear_allocator&& other) noexcept
    : buffer(std::exchange(other.buffer, nullptr)),
    buffer_size(std::exchange(other.buffer_size, 0)),
    buffer_offset(std::exchange(other.buffer_offset, 0))
{}

linear_allocator::linear_allocator(std::size_t size)
    : buffer_size(size),
    buffer_offset(0)
{
    buffer = static_cast<std::byte*>(std::malloc(size));
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }
}

linear_allocator::linear_allocator(std::size_t size, std::size_t align)
    : buffer_size(size),
    buffer_offset(0)
{
    buffer = static_cast<std::byte*>(std::aligned_alloc(align, size));
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }
}

linear_allocator& linear_allocator::operator=(linear_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer != nullptr) {
            std::free(buffer);
            buffer = nullptr;
        }
        buffer = std::exchange(other.buffer, nullptr);
        buffer_size = std::exchange(other.buffer_size, 0);
        buffer_offset = std::excahnge(other.buffer_offset, 0);
    }
    return *this;
}

linear_allocator::~linear_allocator() noexcept
{
    if (buffer != nullptr) {
        std::free(buffer);
        buffer = nullptr;
    }
    buffer_size = 0;
    buffer_offset = 0;
}

void* linear_allocator::allocate(std::size_t size, std::size_t align)
{
    if (!is_power_of_two(align)) {
        throw alignment_error("align must be a power of two");
    }
    std::byte* offset_addr = buffer + buffer_offset;
    std::uintptr_t align_padding = -reinterpret_cast<std::uintptr_t>(offset_addr) & (align - 1);
    if (buffer_offset + align_padding + size > buffer_size) {
        throw std::bad_alloc();
    }
    void* output = static_cast<void*>(offset_addr + align_padding);
    buffer_offset += align_padding + size;
    return output;
}

void* linear_allocator::allocate(std::size size)
{
    if (buffer_offset + size > buffer_size) {
        std::bad_alloc();
    }
    void* output = static_cast<void*>(buffer + buffer_offset);
    buffer_offset += size;
    return output;
}

void* linear_allocator::try_allocate(std::size_t size, std::size_t align) noexcept
{
    if (!is_power_of_two(align)) {
        return nullptr;
    }
    std::byte* offset_addr = buffer + buffer_offset;
    std::uintptr_t align_padding = -reinterpret_cast<std::uintptr_t>(offset_addr) & (align - 1);
    if (buffer_offset + align_padding + size > buffer_size) {
        return nullptr;
    }
    void* output = static_cast<void*>(offset_addr + align_padding);
    buffer_offset += align_padding + size;
    return output;
}

void* linear_allocator::try_allocate(std::size_t size) noexcept
{
    if (buffer_offset + size > buffer_size) {
        return nullptr;
    }
    void* output = static_cast<void*>(buffer + buffer_offset);
    buffer_offset += size;
    return output;
}

void linear_allocator::reset() noexcept
{
    buffer_offset = 0;
}

}
