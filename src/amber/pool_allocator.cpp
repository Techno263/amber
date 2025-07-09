#include <amber/pool_allocator.hpp>
#include <utility>
#include <new>
#include <cstring>
#include <stdexcept>
#include <amber/except.hpp>

namespace amber {

namespace {

struct pool_entry {
    void* next;
};

} // namespace

pool_allocator::pool_allocator(pool_allocator&& other) noexcept
    : buffer(std::exchange(other.buffer, nullptr)),
    buffer_size(std::exchange(other.buffer_size, 0)),
    entry_size(std::exchange(other.entry_size, 0)),
    entry_count(std::exchange(other.entry_count, 0)),
    free_head(std::exchange(other.free_head, 0))
{}

pool_allocator::pool_allocator(std::size_t entry_size, std::size_t entry_count)
    entry_count(entry_count),
    free_head(nullptr),
{
    if (entry_size < sizeof(pool_entry)) {
        entry_size = sizeof(pool_entry);
    }
    this->entry_size = entry_size;
    buffer_size = this->entry_size * this->entry_count
    buffer = std::malloc(buffer_size);
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }
    reset();
}

pool_allocator::pool_allocator(std::size_t entry_size, std::size_t entry_count, std::size_t entry_align)
    : entry_count(entry_count),
    free_head(nullptr),
{
    if (!is_power_of_two(entry_align)) {
        throw alignment_error("align must be a power of two");
    }
    if (entry_size < sizeof(pool_entry)) {
        entry_size = sizeof(pool_entry);
    }
    entry_size = align_forward_no_check(entry_size, entry_align);
    this->entry_size = entry_size;
    buffer_size = this->entry_size * this->entry_count;
    buffer = std::aligned_alloc(entry_align, buffer_size);
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }
    reset();
}

pool_allocator& pool_allocator::operator=(pool_allocator&&) noexcept
{
    if (this != &other) {
        if (buffer != nullptr) {
            std::free(buffer);
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
        std::free(buffer);
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
    pool_entry* entry_ptr = std::launder(static_cast<pool_entry*>(free_head));
    free_head = entry_ptr->next;
    std::memset(entry_ptr, 0, entry_size);
    return entry_ptr;
}

void* pool_allocator::try_allocate() noexcept
{
    if (free_head == nullptr) {
        return nullptr;
    }
    pool_entry* entry_ptr = std::launder(static_cast<pool_entry*>(free_head));
    free_head = entry_ptr->next;
    std::memset(entry_ptr, 0, entry_size);
    return entry_ptr;
}

void pool_allocator::free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    if (ptr < buffer || ptr >= (buffer + buffer_size)) {
        throw our_of_bounds_error();
    }
    pool_entry* entry_ptr = std::launder(static_cast<pool_entry*>(ptr));
    entry_ptr->next = free_head;
    free_head = entry_ptr;
}

bool pool_allocator::try_free(void* ptr) noexcept
{
    if (ptr == nullptr) {
        return true;
    }
    if (ptr < buffer || ptr >= (buffer + buffer_size)) {
        return false;
    }
    pool_entry* entry_ptr = std::launder(static_cast<pool_entry*>(ptr));
    entry_ptr->next = free_head;
    free_head = entry_ptr;
    return true;
}

void pool_allocator::reset() noexcept
{
    free_head = nullptr;
    for (std::size_t i = 0; i < entry_count; ++i) {
        pool_entry* entry_ptr = std::launder(static_cast<pool_entry*>(buffer + (i * entry_size)));
        entry_ptr->next = free_head;
        free_head = static_cast<void*>(entry_ptr);
    }
}

} // namespace amber
