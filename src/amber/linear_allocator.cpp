#include <amber/except.hpp>
#include <amber/linear_allocator.hpp>
#include <amber/util.hpp>
#include <bit>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <utility>

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
        buffer_offset = std::exchange(other.buffer_offset, 0);
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
    if (!std::has_single_bit(align)) {
        throw alignment_error("align must be a power of two");
    }
    std::byte* offset_ptr = buffer + buffer_offset;
    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    std::uintptr_t aligned_addr = align_forward_no_check(offset_addr, static_cast<std::uintptr_t>(align));
    if (aligned_addr + size > buffer_size) {
        throw std::bad_alloc();
    }
    std::uintptr_t padding = aligned_addr - offset_addr;
    buffer_offset += padding + size;
    return reinterpret_cast<void*>(aligned_addr);
}

void* linear_allocator::allocate(std::size_t size)
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
    if (!std::has_single_bit(align)) {
        return nullptr;
    }
    std::byte* offset_ptr = buffer + buffer_offset;
    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    std::uintptr_t aligned_addr = align_forward_no_check(offset_addr, static_cast<std::uintptr_t>(align));
    if (aligned_addr + size > buffer_size) {
        return nullptr;
    }
    std::uintptr_t padding = aligned_addr - offset_addr;
    buffer_offset += padding + size;
    return reinterpret_cast<void*>(aligned_addr);
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
