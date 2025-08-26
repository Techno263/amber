#include <amber/malloc_buffer.hpp>
#include <mica/mica.hpp>
#include <utility>

namespace amber {

malloc_buffer::malloc_buffer(malloc_buffer&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    size_(std::exchange(other.size_, 0))
{}

malloc_buffer& malloc_buffer::operator=(malloc_buffer&& other) noexcept
{
    if (this != &other) {
        if (buffer_ != nullptr) {
            std::free(buffer_);
            buffer_ = nullptr;
        }
        buffer_ = std::exchange(other.buffer_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}

malloc_buffer::~malloc_buffer() noexcept
{
    if (buffer_ != nullptr) {
        std::free(buffer_);
        buffer_ = nullptr;
    }
    size_ = 0;
}

std::expected<malloc_buffer, std::string> malloc_buffer::create(std::size_t size) noexcept
{
    std::byte* buffer = static_cast<std::byte*>(std::malloc(size));
    if (buffer == nullptr) [[unlikely]] {
        auto&& exp_msg = mica::format("malloc failed, size: {}", size);
        if (!exp_msg.has_value()) [[unlikely]] {
            return std::unexpected("formatting failed while handling malloc error");
        }
        return std::unexpected(std::move(exp_msg).value());
    }
    return malloc_buffer(buffer, size);
}

std::span<std::byte> malloc_buffer::buffer() noexcept
{
    return std::span(buffer_, size_);
}

const std::span<std::byte> malloc_buffer::buffer() const noexcept
{
    return std::span(buffer_, size_);
}

std::size_t malloc_buffer::size() const noexcept
{
    return size_;
}

malloc_buffer::malloc_buffer(std::byte* buffer, std::size_t size) noexcept
    : buffer_(buffer),
    size_(size)
{}

} // namespace amber
