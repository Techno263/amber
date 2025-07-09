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
    std::size_t padding;
};

std::size_t calc_alloc_header_size(const stack_allocator_header& header)
{
    if (header < 0xff) {
        return 1;
    } else {
        return 1 + sizeof(std::size_t);
    }
}

std::size_t calc_padding_with_header(std::uintptr_t offset_addr, std::size_t alignment, std::size_t header_size)
{
    
}

void write_alloc_header(std::byte* alloc_ptr, const stack_allocator_header& header)
{
    if (header.padding < 0xff) {
        // use 1-byte header
        alloc_ptr[-1] = static_cast<std::byte>(header.padding);
    } else {
        // use large header
        alloc_ptr[-1] = std::byte{0xff};
        std::memcpy(alloc_ptr - 1 - sizeof(std::size_t), &header.padding, sizeof(std::size_t));
    }
}

stack_allocator_header read_alloc_header(std::byte* alloc_ptr)
{
    std::size_t padding = 0;
    std::byte byte1 = alloc_ptr[-1];
    if (byte1 == 0xff) {
        // read large header
        std::memcpy(&padding, alloc_ptr - 1 - sizeof(std::size_t), sizeof(std::size_t));
    } else {
        // read 1-byte header
        padding = static_cast<std::size_t>(alloc_ptr[-1]);
    }
    alloc_header output{.padding = padding}
    return output;
}

}

stack_allocator::stack_allocator(stack_allocator&& other) noexcept
    : buffer(std::exchange(other.buffer, nullptr)),
    buffer_size(std::exchange(other.buffer_size, 0)),
    buffer_offset(std::exchange(other.buffer_offset, 0))
{}

stack_allocator::stack_allocator(std::size_t size)
    : buffer_size(size),
    buffer_offset(0)
{
    buffer = static_cast<std::byte*>(std::malloc(size));
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }
}

stack_allocator::stack_allocator(std::size_t size, std::size_t align)
    : buffer_size(size),
    buffer_offset(0)
{
    buffer = static_cast<std::byte*>(std::aligned_alloc(align, size));
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }
}

stack_allocator& stack_allocator::operator=(linear_allocator&& other) noexcept
{
    if (this != &other) {
        if (buffer != nullptr) {
            std::free(buffer);
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
        std::free(buffer);
        buffer = nullptr;
    }
    buffer_size = 0;
    buffer_offset = 0;
}

void* stack_allocator::allocate(std::size_t size, std::size_t align)
{
    if (align <= 0 || (align & (align - 1)) != 0) {
        throw std::runtime_error("align must be a positive power of two");
    }
    std::byte* offset_addr = buffer + buffer_offset;
    std::uintptr_t align_padding = -reinterpret_cast<std::uintptr_t>(offset_addr) & (align - 1);
    std::size_t header_size = calc_alloc_header_size(


    if (buffer_offset + align_padding + size > buffer_size) {
        throw std::bad_alloc();
    }
    void* output = static_cast<void*>(offset_addr + align_padding);
    buffer_offset +=
}

}
