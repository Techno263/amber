#include <amber/except.hpp>
#include <amber/pool_allocator.hpp>
#include <amber/util.hpp>
#include <bit>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

namespace amber {

namespace {

struct pool_entry {
public:
    pool_entry(std::byte* next) noexcept
        : next(next)
    {}

    std::byte* next;
};
// Sanity check
static_assert(std::has_single_bit(alignof(pool_entry)));
static_assert(std::is_nothrow_constructible_v<pool_entry, decltype(free_head)>);

} // namespace

pool_allocator::pool_allocator(pool_allocator&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    free_head_(std::exchange(other.free_head_, nullptr)),
    buffer_size_(std::exchange(other.buffer_size_, 0)),
    entry_size_(std::exchange(other.entry_size_, 0)),
    entry_count_(std::exchange(other.entry_count_, 0))
{}

std::expected<pool_allocator, alloc_error> pool_allocator::create(
    std::size_t buffer_alignment,
    std::size_t entry_alignment,
    std::size_t entry_size,
    std::size_t entry_count
) noexcept
{
    if (!std::has_single_bit(buffer_alignment)) [[unlikely]] {
        return std::unexpected(alloc_error::alignment_error);
    }
    if (!std::has_single_bit(entry_alignment)) [[unlikely]] {
        return std::unexpected(alloc_error::alignment_error);
    }
    // can use max here since both alignments are guaranteed to be powers of two, otherwise LCM should be used
    entry_alignment = std::max(entry_alignment, alignof(pool_entry));
    buffer_alignment = std::max(buffer_alignment, entry_alignment);
    entry_size = std::max(entry_size, sizeof(pool_entry));
    entry_size = align_forward(entry_alignment, entry_size);
    std::size_t buffer_size = entry_size * entry_count;
    auto exp_buffer = aligned_alloc(entry_alignment, buffer_size);
    if (!exp_buffer.has_value()) [[unlikely]] {
        return std::unexpected(exp.error());
    }
    std::byte* buffer = static_cast<std::byte*>(exp_buffer.value());

    std::byte* free_head = nullptr;
    for (std::size_t i = 0; i < entry_count; ++i) {
        pool_entry* entry_ptr = reinterpret_cast<pool_entry*>(buffer + (i * entry_size));
        entry_ptr = std::assume_aligned<alignof(pool_entry)>(entry_ptr);
        entry_ptr = std::launder(std::construct_at(entry_ptr, free_head));
        free_head = reinterpret_cast<std::byte*>(entry_ptr);
    }

    // Always return constructor for Return Value Optimization (RVO) using copy elision
    return pool_allocator(buffer, free_head, buffer_size, entry_size, entry_count);
}

std::expected<pool_alloctor, alloc_error> pool_allocator::create(
    std::size_t entry_alignment,
    std::size_t entry_size,
    std::size_t entry_count
) noexcept
{
    // Always directly return `create` for RVO
    return create(entry_alignment, entry_alignment, entry_size, entry_count);
}

std::expected<pool_allocator, alloc_error> pool_allocator::create(
    std::size_t entry_size, std::size_t entry_count) noexcept
{
    // Always directly return `create` for RVO
    return create(alignof(std::max_align_t), 1, entry_size, entry_count);
}

template<typename T>
std::expected<pool_allocator, alloc_error> create(std::size_t entry_count) noexcept
{
    // Always directly return `create` for RVO
    return create(
        std::max(alignof(T), alignof(std::max_align_t)),
        alignof(T),
        sizeof(T),
        entry_count
    );
}

pool_allocator& pool_allocator::operator=(pool_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer_ != nullptr) {
            aligned_free(buffer_);
        }
        buffer_ = std::exchange(other.buffer_, nullptr);
        free_head_ = std::exchange(other.free_head_, nullptr);
        buffer_size_ = std::exchange(other.buffer_size_, 0);
        entry_size_ = std::exchange(other.entry_size_, 0);
        entry_count_ = std::exchange(other.entry_count_, 0);
    }
    return *this;
}

pool_allocator::~pool_allocator() noexcept
{
    if (buffer_ != nullptr) {
        aligned_free(buffer_);
        buffer_ = nullptr;
    }
    free_head_ = nullptr;
    buffer_size_ = 0;
    entry_size_ = 0;
    entry_count_ = 0;
}

std::expected<void* alloc_error> pool_allocator::allocate()
{
    if (free_head_ == nullptr) [[unlikely]] {
        return std::unexpected(alloc_error::out_of_capacity_error);
    }
    pool_entry* entry_ptr = reinterpret_cast<pool_entry*>(free_head_);
    entry_ptr = std::assume_aligned<alignof(pool_entry)>(entry_ptr);
    free_head_ = entry_ptr->next;
    std::memset(entry_ptr, 0, entry_size_);
    return entry_ptr;
}

void pool_allocator::free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    pool_entry* entry_ptr = std::assume_aligned<alignof(pool_entry)>(static_cast<pool_entry*>(ptr));
    entry_ptr = std::launder(std::construct_at(entry_ptr, free_head_));
    free_head_ = reinterpret_cast<std::byte*>(entry_ptr);
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

pool_allocator::pool_allocator(
    std::byte* buffer, std::byte* free_head, std::size_t buffer_size, std::size_t entry_size, std::size_t entry_count) noexcept
    : buffer_(buffer),
    free_head(_free_head),
    buffer_size(_buffer_size),
    entry_size_(entry_size),
    entry_count_(entry_count)
{}

} // namespace amber
