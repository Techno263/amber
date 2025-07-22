#include <amber/alloc_error.hpp>
#include <amber/linear_allocator.hpp>
#include <amber/util.hpp>
#include <bit>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <utility>

namespace amber {

linear_allocator::linear_allocator(linear_allocator&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    buffer_size_(std::exchange(other.buffer_size_, 0)),
    buffer_offset_(std::exchange(other.buffer_offset_, 0))
{}

std::expected<linear_allocator, alloc_error> linear_allocator::create(
    std::size_t alignment, std::size_t size) noexcept
{
    auto exp_buffer = aligned_alloc(alignment, size);
    if (!exp_buffer.has_value()) [[unlikely]] {
        return std::unexpected(exp_buffer.error());
    }
    std::byte* buffer = reinterpret_cast<std::byte*>(exp_buffer.value());
    // Always return constructor for Return Value Optimization (RVO) using copy elision
    return linear_allocator(buffer, size, 0);
}

std::expected<linear_allocator, alloc_error> linear_allocator::create(
    std::size_t size) noexcept
{
    // Always directly return `create` for RVO
    return create(alignof(std::max_align_t), size);
}

linear_allocator& linear_allocator::operator=(linear_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer_ != nullptr) {
            aligned_free(buffer_);
            buffer_ = nullptr;
        }
        buffer_ = std::exchange(other.buffer_, nullptr);
        buffer_size_ = std::exchange(other.buffer_size_, 0);
        buffer_offset_ = std::exchange(other.buffer_offset_, 0);
    }
    return *this;
}

linear_allocator::~linear_allocator() noexcept
{
    if (buffer_ != nullptr) {
        aligned_free(buffer_);
        buffer_ = nullptr;
    }
    buffer_size_ = 0;
    buffer_offset_ = 0;
}

std::expected<void*, alloc_error> linear_allocator::allocate(std::size_t alignment, std::size_t size) noexcept
{
    if (!std::has_single_bit(alignment)) [[unlikely]] {
        return std::unexpected(alloc_error::alignment_error);
    }
    std::byte* offset_ptr = buffer_ + buffer_offset_;
    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    std::uintptr_t aligned_addr = align_forward(static_cast<std::uintptr_t>(alignment), offset_addr);
    std::uintptr_t aligned_padding = aligned_addr - offset_addr;
    if (buffer_offset_ + aligned_padding + size > buffer_size_) [[unlikely]] {
        return std::unexpected(alloc_error::bad_alloc_error);
    }
    buffer_offset_ += aligned_padding + size;
    return reinterpret_cast<void*>(aligned_addr);
}

std::expected<void*, alloc_error> linear_allocator::allocate(std::size_t size) noexcept
{
    return allocate(alignof(std::max_align_t), size);
}

void linear_allocator::reset() noexcept
{
    buffer_offset_ = 0;
}

std::size_t linear_allocator::buffer_size() const noexcept
{
    return buffer_size_;
}

std::size_t linear_allocator::buffer_offset() const noexcept
{
    return buffer_offset_;
}

linear_allocator::linear_allocator(std::byte* buffer, std::size_t buffer_size, std::size_t buffer_offset) noexcept
    : buffer_(buffer),
    buffer_size_(buffer_size),
    buffer_offset_(buffer_offset)
{}

} // namespace amber
