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
    : linear_allocator(alignof(std::max_align_t), size)
{}

linear_allocator::linear_allocator(std::size_t alignment, std::size_t size)
    : buffer_size(size),
    buffer_offset(0)
{
    buffer = static_cast<std::byte*>(aligned_malloc(alignment, size));
}

linear_allocator& linear_allocator::operator=(linear_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer != nullptr) {
            aligned_free(buffer);
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
        aligned_free(buffer);
        buffer = nullptr;
    }
    buffer_size = 0;
    buffer_offset = 0;
}

void* linear_allocator::allocate(std::size_t alignment, std::size_t size)
{
    if (!std::has_single_bit(alignment)) {
        throw alignment_error("alignment must be a power of two");
    }
    std::byte* offset_ptr = buffer + buffer_offset;
    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    std::uintptr_t aligned_addr = align_forward_no_check(static_cast<std::uintptr_t>(alignment), offset_addr);
    std::uintptr_t aligned_padding = aligned_addr - offset_addr;
    if (buffer_offset + aligned_padding + size > buffer_size) {
        throw std::bad_alloc();
    }
    buffer_offset += aligned_padding + size;
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

void* linear_allocator::try_allocate(std::size_t alignment, std::size_t size) noexcept
{
    if (!std::has_single_bit(alignment)) {
        return nullptr;
    }
    std::byte* offset_ptr = buffer + buffer_offset;
    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    std::uintptr_t aligned_addr = align_forward_no_check(static_cast<std::uintptr_t>(alignment), offset_addr);
    std::uintptr_t aligned_padding = aligned_addr - offset_addr;
    if (buffer_offset + aligned_padding + size > buffer_size) {
        return nullptr;
    }
    buffer_offset += aligned_padding + size;
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

std::size_t linear_allocator::size() const
{
    return buffer_size;
}

std::size_t linear_allocator::offset() const
{
    return buffer_offset;
}

} // namespace amber
