#include <amber/stack_allocator.hpp>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <bit>
#include <amber/util.hpp>

namespace amber {

namespace {

struct alloc_header {
public:
    alloc_header(std::size_t padding)
        : padding(padding)
    {}

    std::size_t padding;
};
// The size of the header must be a multiple of its alignment
// This should already be guaranteed, but added a sanity check
static_assert(sizeof(alloc_header) % alignof(alloc_header) == 0);

enum class allocate_status {
    success,
    bad_alloc,
};

struct allocate_result {
public:
    allocate_result(void* ptr, allocate_status status) noexcept
        : ptr(ptr), status(status)
    {}

    void* ptr;
    allocate_status status;
};

allocate_result allocate_helper(stack_allocator* a, std::size_t size) noexcept
{
    if (a->buffer_offset + size > a->buffer_size) {
        return allocate_result(nullptr, allocate_status::bad_alloc);
    }
    void* output = static_cast<void*>(a->buffer + a->buffer_offset);
    a->buffer_offset += size;
    return allocate_result(output, allocate_status::success);
}

enum class allocate_align_status {
    success,
    bad_alloc,
    alignment_error,
};

struct allocate_align_result {
public:
    allocate_align_result(void* ptr, allocate_align_status status) noexcept
        : ptr(ptr), status(status)
    {}

    void* ptr;
    allocate_align_status status;
};

allocate_align_result allocate_align_helper(stack_allocator* a, std::size_t alignment, std::size_t size) noexcept
{
    if (!std::has_single_bit(alignment)) {
        return allocate_align_result(nullptr, allocate_align_status::alignment_error);
    }
    if (alignment < alignof(alloc_header)) {
        alignment = alignof(alloc_header);
    }
    if (a->buffer_offset != 0) {
        // Add alloc_header when not at start of buffer
        std::byte* offset_ptr = a->buffer + a->buffer_offset;
        std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
        std::uintptr_t target_addr = offset_addr + sizeof(alloc_header);
        std::uintptr_t aligned_addr = align_forward_no_check(static_cast<std::uintptr_t>(alignment), target_addr);
        std::uintptr_t aligned_padding = aligned_addr - offset_addr;
        if (a->buffer_offset + aligned_padding + size > a->buffer_size) {
            return allocate_align_result(nullptr, allocate_align_status::bad_alloc);
        }
        // Create and set alloc_header
        alloc_header* header_ptr = reinterpret_cast<alloc_header*>(aligned_addr - sizeof(alloc_header));
        header_ptr = std::assume_aligned<alignof(alloc_header)>(header_ptr);
        header_ptr = std::launder(std::construct_at(header_ptr, aligned_padding));

        a->buffer_offset += aligned_padding + size;
        return allocate_align_result(reinterpret_cast<void*>(aligned_addr), allocate_align_status::success);
    } else {
        // At start of buffer
        if 
        // Handle alloc at start of buffer
        if (size > a->buffer_size) {
            return allocate_align_result(nullptr, allocate_align_status::bad_alloc);
        }
        a->buffer_offset += size;
        return allocate_align_result(static_cast<void*>(a->buffer), allocate_align_status::success);
    }
}

} // namespace <unnamed>

stack_allocator::stack_allocator(stack_allocator&& other) noexcept
    : buffer(std::exchange(other.buffer, nullptr)),
    buffer_size(std::exchange(other.buffer_size, 0)),
    buffer_offset(std::exchange(other.buffer_offset, 0))
{}

stack_allocator::stack_allocator(std::size_t size)
    : stack_allocator(alignof(std::max_align_t), size)
{}

stack_allocator::stack_allocator(std::size_t alignment, std::size_t size)
    : buffer_size(size),
    buffer_offset(0)
{
    buffer = static_cast<std::byte*>(aligned_malloc(alignment, size));
}

stack_allocator& stack_allocator::operator=(linear_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer != nullptr) {
            aligned_free(buffer);
        }
        buffer = std::exchange(other.buffer, nullptr);
        buffer_size = std::exchange(other.buffer_size, 0);
        buffer_offset = std::exchange(other.buffer_offset, 0);
    }
    return *this;
}

stack_allocator::~stack_allocator() noexcept
{
    if (buffer != nullptr) {
        aligned_free(buffer);
        buffer = nullptr;
    }
    buffer_size = 0;
    buffer_offset = 0;
}

void* stack_allocator::allocate(std::size_t alignment, std::size_t size)
{
    allocate_align_result res = allocate_align_helper(this, alignment, size);
    switch (res.status) {
    case allocate_align_status::success:
        return res.ptr;
    case allocate_align_status::bad_alloc:
        throw std::bad_alloc();
    case allocate_align_status::alignment_error:
        throw alignment_error("alignment must be a power of two");
    }

    //if (!std::has_single_bit(align)) {
    //    throw alignment_error("alignment must be a power of two");
    //}
    //if (align < alignof(alloc_header)) {
    //    align = alignof(alloc_header);
    //}
    //if (buffer_offset != 0) {
    //    // Add alloc_header when not at start of buffer
    //    std::byte* offset_ptr = buffer + buffer_offset;
    //    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    //    std::uintptr_t target_addr = offset_addr + sizeof(alloc_header);
    //    std::uintptr_t aligned_addr = align_forward_no_check(static_cast<std::uintptr_t>(align), target_addr);
    //    std::uintptr_t aligned_padding = aligned_addr - offset_addr;
    //    if (buffer_offset + aligned_padding + size > buffer_size) {
    //        throw std::bad_alloc();
    //    }
    //    // Create and set alloc_header
    //    alloc_header* header_ptr = reinterpret_cast<alloc_header*>(aligned_addr - sizeof(alloc_header));
    //    header_ptr = std::assume_aligned<alignof(alloc_header)>(header_ptr);
    //    header_ptr = std::launder(std::construct_at(header_ptr, aligned_padding));

    //    buffer_offset += aligned_padding + size;
    //    return reinterpret_cast<void*>(aligned_addr);
    //} else {
    //    // Handle alloc at start of buffer
    //    if (size > buffer_size) {
    //        throw std::bad_alloc();
    //    }
    //    buffer_offset += size;
    //    return static_cast<void*>(buffer);
    //}
}

void* stack_allocator::allocate(std::size_t size)
{
    allocate_result res = allocate_helper(this, size);
    switch(res.status) {
    case allocate_status::success:
        return res.ptr;
    case allocate_status::bad_alloc:
        throw std::bad_alloc();
    }
    //if (buffer_offset + size > buffer_size) {
    //    throw std::bad_alloc();
    //}
    //void* output = static_cast<void*>(buffer + buffer_offset);
    //buffer_offset += size;
    //return output;
}

void* stack_allocator::try_allocate(std::size_t alignment, std:size_t size) noexcept
{
    allocate_align_result res = allocate_align_helper(this, alignment, size);
    switch (res.status) {
    case allocate_align_status::success:
        return res.ptr;
    default:
        return nullptr;
    }
    //if (!std::has_single_bit(align)) {
    //    return nullptr;
    //}
    //if (align < alignof(alloc_header)) {
    //    align = alignof(alloc_header);
    //}
    //if (buffer_offset != 0) {
    //    // Add alloc_header when not at start of buffer
    //    std::byte* offset_ptr = buffer + buffer_offset;
    //    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    //    std::uintptr_t target_addr = offset_addr + sizeof(alloc_header);
    //    std::uintptr_t aligned_addr = align_forward_no_check(static_cast<std::uintptr_t>(align), target_addr);
    //    std::uintptr_t aligned_padding = aligned_addr - offset_addr;
    //    if (buffer_offset + aligned_padding + size > buffer_size) {
    //        return nullptr;
    //    }
    //    // Create and set alloc_header
    //    alloc_header* header_ptr = reinterpret_cast<alloc_header*>(aligned_addr - sizeof(alloc_header));
    //    header_ptr = std::assume_aligned<alignof(alloc_header)>(header_ptr);
    //    header_ptr = std::launder(std::construct_at(header_ptr, aligned_padding));

    //    buffer_offset += aligned_padding + size;
    //    return reinterpret_cast<void*>(aligned_addr);
    //} else {
    //    // Handle alloc at start of buffer
    //    if (size > buffer_size) {
    //        throw std::bad_alloc();
    //    }
    //    buffer_offset += size;
    //    return static_cast<void*>(buffer);
    //}
}

void* stack_allocator::try_allocate(std::size_t size) noexcept
{
    allocate_result res = allocate_helper(this, size);
    switch(res.status) {
    case allocate_status::success:
        return res.ptr;
    default:
        return nullptr;
    }
    //if (buffer_offset + size > buffer_size) {
    //    throw std::bad_alloc();
    //}
    //void* output = static_cast<void*>(buffer + buffer_offset);
    //buffer_offset += size;
    //return output;
}

void stack_allocator::free(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(ptr);
}

}
