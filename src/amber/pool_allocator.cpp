#include <amber/pool_allocator.hpp>
#include <cstring>
#include <memory>
#include <new>
#include <utility>

namespace amber {

namespace internal {

pool_entry::pool_entry(std::byte* next) noexcept
    : next(next)
{}

} // namespace amber::internal

pool_allocator::pool_allocator(pool_allocator&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    free_head_(std::exchange(other.free_head_, nullptr)),
    buffer_size_(std::exchange(other.buffer_size_, 0)),
    entry_size_(std::exchange(other.entry_size_, 0)),
    entry_count_(std::exchange(other.entry_count_, 0)),
    entry_allocate_count_(std::exchange(other.entry_allocate_count_, 0))
{}

pool_allocator::~pool_allocator() noexcept
{
    buffer_ = nullptr;
    free_head_ = nullptr;
    buffer_size_ = 0;
    entry_size_ = 0;
    entry_count_ = 0;
    entry_allocate_count_ = 0;
}

pool_allocator& pool_allocator::operator=(pool_allocator&& other) noexcept
{
    if (this != &other) {
        buffer_ = std::exchange(other.buffer_, nullptr);
        free_head_ = std::exchange(other.free_head_, nullptr);
        buffer_size_ = std::exchange(other.buffer_size_, 0);
        entry_size_ = std::exchange(other.entry_size_, 0);
        entry_count_ = std::exchange(other.entry_count_, 0);
        entry_allocate_count_ = std::exchange(other.entry_allocate_count_, 0);
    }
    return *this;
}

std::expected<void*, std::string> pool_allocator::allocate() noexcept
{
    if (free_head_ == nullptr) [[unlikely]] {
        return std::unexpected("out of capacity");
    }
    internal::pool_entry* entry_ptr = reinterpret_cast<internal::pool_entry*>(free_head_);
    entry_ptr = std::assume_aligned<alignof(internal::pool_entry)>(entry_ptr);
    free_head_ = entry_ptr->next;
    std::memset(reinterpret_cast<void*>(entry_ptr), 0, entry_size_);
    entry_allocate_count_ += 1;
    return entry_ptr;
}

void pool_allocator::free(void* ptr) noexcept
{
    if (ptr == nullptr) {
        return;
    }
    internal::pool_entry* entry_ptr = std::assume_aligned<alignof(internal::pool_entry)>(
        static_cast<internal::pool_entry*>(ptr)
    );
    entry_ptr = std::launder(std::construct_at(entry_ptr, free_head_));
    free_head_ = reinterpret_cast<std::byte*>(entry_ptr);
    entry_allocate_count_ -= 1;
}

std::size_t pool_allocator::buffer_size() const noexcept
{
    return buffer_size_;
}

std::size_t pool_allocator::entry_size() const noexcept
{
    return entry_size_;
}

std::size_t pool_allocator::entry_count() const noexcept
{
    return entry_count_;
}

std::size_t pool_allocator::entry_allocate_count() const noexcept
{
    return entry_allocate_count_;
}

std::size_t pool_allocator::entry_free_count() const noexcept
{
    return entry_count_ - entry_allocate_count_;
}

pool_allocator::pool_allocator(
    std::byte* buffer,
    std::byte* free_head,
    std::size_t buffer_size,
    std::size_t entry_size,
    std::size_t entry_count,
    std::size_t entry_allocate_count
) noexcept
    : buffer_(buffer),
    free_head_(free_head),
    buffer_size_(buffer_size),
    entry_size_(entry_size),
    entry_count_(entry_count),
    entry_allocate_count_(entry_allocate_count)
{}

} // namespace amber
