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

} // namespace

pool_allocator::pool_allocator(pool_allocator&& other) noexcept
    : buffer(std::exchange(other.buffer, nullptr)),
    free_head(std::exchange(other.free_head, nullptr)),
    buffer_size(std::exchange(other.buffer_size, 0)),
    entry_size(std::exchange(other.entry_size, 0)),
    entry_count(std::exchange(other.entry_count, 0))
{}

pool_allocator::pool_allocator(std::size_t entry_size, std::size_t entry_count)
    : pool_allocator(alignof(std::max_align_t), entry_size, entry_count)
{}

pool_allocator::pool_allocator(std::size_t entry_alignment, std::size_t entry_size, std::size_t entry_count)
    : entry_count(entry_count),
    free_head(nullptr)
{
    if (!std::has_single_bit(entry_alignment)) {
        throw alignment_error("alignment must be a power of two");
    }
    // can use max here since both alignments are guaranteed to be powers of two, otherwise LCM should be used
    entry_alignment = std::max(entry_alignment, alignof(pool_entry));
    if (entry_size < sizeof(pool_entry)) {
        entry_size = sizeof(pool_entry);
    }
    entry_size = align_forward_no_check(entry_alignment, entry_size);
    this->entry_size = entry_size;
    buffer_size = this->entry_size * this->entry_count;
    buffer = static_cast<std::byte*>(try_aligned_malloc_no_check(entry_alignment, buffer_size));
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }

    static_assert(std::is_nothrow_constructible_v<pool_entry, decltype(free_head)>);
    free_head = nullptr;
    for (std::size_t i = 0; i < entry_count; ++i) {
        pool_entry* entry_ptr = reinterpret_cast<pool_entry*>(buffer + (i * entry_size));
        entry_ptr = std::assume_aligned<alignof(pool_entry)>(entry_ptr);
        entry_ptr = std::launder(std::construct_at(entry_ptr, free_head));
        //entry_ptr->next = free_head;
        free_head = reinterpret_cast<std::byte*>(entry_ptr);
    }
}

pool_allocator& pool_allocator::operator=(pool_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer != nullptr) {
            aligned_free(buffer);
        }
        buffer = std::exchange(other.buffer, nullptr);
        free_head = std::exchange(other.free_head, nullptr);
        buffer_size = std::exchange(other.buffer_size, 0);
        entry_size = std::exchange(other.entry_size, 0);
        entry_count = std::exchange(other.entry_count, 0);
    }
    return *this;
}

pool_allocator::~pool_allocator() noexcept
{
    if (buffer != nullptr) {
        aligned_free(buffer);
        buffer = nullptr;
    }
    free_head = nullptr;
    buffer_size = 0;
    entry_size = 0;
    entry_count = 0;
}

void* pool_allocator::allocate()
{
    if (free_head == nullptr) {
        throw out_of_capacity_error();
    }
    pool_entry* entry_ptr = reinterpret_cast<pool_entry*>(free_head);
    entry_ptr = std::assume_aligned<alignof(pool_entry)>(entry_ptr);
    free_head = entry_ptr->next;
    std::memset(entry_ptr, 0, entry_size);
    return entry_ptr;
}

void* pool_allocator::try_allocate() noexcept
{
    if (free_head == nullptr) {
        return nullptr;
    }
    pool_entry* entry_ptr = reinterpret_cast<pool_entry*>(free_head);
    entry_ptr = std::assume_aligned<alignof(pool_entry)>(entry_ptr);
    free_head = entry_ptr->next;
    std::memset(entry_ptr, 0, entry_size);
    return entry_ptr;
}

void pool_allocator::free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    //if (ptr < buffer || ptr >= (buffer + buffer_size)) {
    //    throw out_of_bounds_error();
    //}
    pool_entry* entry_ptr = std::assume_aligned<alignof(pool_entry)>(static_cast<pool_entry*>(ptr));
    entry_ptr = std::launder(std::construct_at(entry_ptr, free_head));
    //entry_ptr->next = free_head;
    free_head = reinterpret_cast<std::byte*>(entry_ptr);
}

bool pool_allocator::try_free(void* ptr) noexcept
{
    static_assert(std::is_nothrow_constructible_v<pool_entry, decltype(free_head)>);
    if (ptr == nullptr) {
        return true;
    }
    //if (ptr < buffer || ptr >= (buffer + buffer_size)) {
    //    return false;
    //}
    pool_entry* entry_ptr = std::assume_aligned<alignof(pool_entry)>(static_cast<pool_entry*>(ptr));
    entry_ptr = std::launder(std::construct_at(entry_ptr, free_head));
    //entry_ptr->next = free_head;
    free_head = reinterpret_cast<std::byte*>(entry_ptr);
    return true;
}

} // namespace amber
