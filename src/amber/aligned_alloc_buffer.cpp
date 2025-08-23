#include <amber/aligned_alloc_buffer.hpp>
#include <cstdlib>
#include <mica/mica.hpp>
#include <utility>

namespace amber {

aligned_buffer::aligned_buffer(aligned_buffer&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    alignment_(std::exchange(other.alignment_, 0)),
    size_(std::exchange(other.size_, 0))
{}

aligned_buffer& aligned_buffer::operator=(aligned_buffer&& other) noexcept
{
    if (this != &other) {
        if (buffer_ != nullptr) {
            std::free(buffer_);
            buffer_ = nullptr;
        }
        buffer_ = std::exchange(other.buffer_, nullptr);
        alignment_ = std::exchange(other.alignment_, 0);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}

aligned_buffer::~aligned_buffer() noexcept
{
    if (buffer_ != nullptr) {
        std::free(buffer_);
        buffer_ = nullptr;
    }
    alignment_ = 0;
    size_ = 0;
}

std::expected<aligned_buffer, std::string> aligned_buffer::create(
    std::size_t alignment,
    std::size_t size
) noexcept
{
    void* buffer = std::aligned_alloc(alignment, size);
    if (buffer == nullptr) [[unlikely]] {
        auto&& exp_msg = mica::format("aligned_alloc failed, alignment: {}, size: {}", alignment, size);
        if (!exp_msg.has_value()) [[unlikely]] {
            return std::unexpected("formatting failed while handling alignment error");
        }
        return std::unexpected(std::move(exp_msg).value());
    }
    return aligned_buffer(buffer, alignment, size);
}

void* aligned_buffer::buffer() noexcept
{
    return buffer_;
}

const void* aligned_buffer::buffer() const noexcept
{
    return buffer_;
}

std::size_t aligned_buffer::alignment() const noexcept
{
    return alignment_;
}

std::size_t aligned_buffer::size() const noexcept
{
    return size_;
}

aligned_buffer::aligned_buffer(void* buffer, std::size_t alignment, std::size_t size) noexcept
    : buffer_(buffer),
    alignment_(alignment),
    size_(size)
{}

} // namespace amber
