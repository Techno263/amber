#include <algorithm>
#include <amber/stack_allocator.hpp>
#include <amber/util.hpp>
#include <bit>
#include <cstdint>
#include <memory>
#include <mica/mica.hpp>
#include <new>
#include <string>
#include <utility>

namespace amber {

namespace {

struct alloc_header {
public:
    alloc_header(std::size_t padding)
        : padding(padding)
    {}

    std::size_t padding;
};
static_assert(std::has_single_bit(alignof(alloc_header)));

} // unnamed namespace

stack_allocator::stack_allocator(stack_allocator&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    buffer_size_(std::exchange(other.buffer_size_, 0)),
    buffer_offset_(std::exchange(other.buffer_offset_, 0))
{}

stack_allocator& stack_allocator::operator=(stack_allocator&& other) noexcept
{
    if (this != &other) {
        buffer_ = std::exchange(other.buffer_, nullptr);
        buffer_size_ = std::exchange(other.buffer_size_, 0);
        buffer_offset_ = std::exchange(other.buffer_offset_, 0);
    }
    return *this;
}

stack_allocator::~stack_allocator() noexcept
{
    buffer_ = nullptr;
    buffer_size_ = 0;
    buffer_offset_ = 0;
}

std::expected<void*, std::string> stack_allocator::allocate(
    std::size_t alignment, std::size_t size) noexcept
{
    if (!std::has_single_bit(alignment)) [[unlikely]] {
        auto&& exp_msg = mica::format("invalid alignment: {}", alignment);
        if (!exp_msg.has_value()) [[unlikely]] {
            return std::unexpected("formatting failed while handling alignment error");
        }
        return std::unexpected(std::move(exp_msg).value());
    }
    alignment = std::max(alignof(alloc_header), alignment);
    std::byte* offset_ptr = buffer_ + buffer_offset_;
    std::uintptr_t offset_addr = reinterpret_cast<std::uintptr_t>(offset_ptr);
    std::uintptr_t target_addr = offset_addr + sizeof(alloc_header);
    std::uintptr_t aligned_addr = align_forward(static_cast<std::uintptr_t>(alignment), target_addr);
    std::uintptr_t aligned_padding = aligned_addr - offset_addr;
    if (buffer_offset_ + aligned_padding + size > buffer_size_) [[unlikely]] {
        return std::unexpected("out of capacity");
    }

    alloc_header* header_ptr = reinterpret_cast<alloc_header*>(aligned_addr - sizeof(alloc_header));
    header_ptr = std::assume_aligned<alignof(alloc_header)>(header_ptr);
    header_ptr = std::launder(std::construct_at(header_ptr, aligned_padding));

    buffer_offset_ += aligned_padding + size;
    return reinterpret_cast<void*>(aligned_addr);
}

std::expected<void*, std::string> stack_allocator::allocate(std::size_t size) noexcept
{
    return allocate(alignof(std::max_align_t), size);
}

void stack_allocator::free(void* ptr) noexcept
{
    if (ptr == nullptr) {
        return;
    }
    std::uintptr_t aligned_addr = reinterpret_cast<std::uintptr_t>(ptr);
    std::uintptr_t buffer_addr = reinterpret_cast<std::uintptr_t>(buffer_);
    alloc_header* header_ptr = reinterpret_cast<alloc_header*>(aligned_addr - sizeof(alloc_header));
    std::uintptr_t aligned_padding = header_ptr->padding;
    std::uintptr_t new_offset = aligned_addr - aligned_padding - buffer_addr;
    buffer_offset_ = new_offset;
}

std::size_t stack_allocator::buffer_size() const noexcept
{
    return buffer_size_;
}

std::size_t stack_allocator::buffer_offset() const noexcept
{
    return buffer_offset_;
}

stack_allocator::stack_allocator(
    std::byte* buffer, std::size_t buffer_size, std::size_t buffer_offset) noexcept
    : buffer_(buffer),
    buffer_size_(buffer_size_),
    buffer_offset_(buffer_offset)
{}

} // namespace amber
