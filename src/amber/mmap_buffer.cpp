#include <amber/bitwise_enum.hpp>
#include <amber/mmap_buffer.hpp>
#include <cstring>
#include <mica/mica.hpp>
#include <type_traits>
#include <utility>

namespace amber {

enum class mmap_prot : int {
    exec = PROT_EXEC,
    read = PROT_READ,
    write = PROT_WRITE,
    none = PROT_NONE,
};
AMBER_BITWISE_ENUM(mmap_prot);

mmap_buffer::mmap_buffer(mmap_buffer&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)),
    size_(std::exchange(other.size_, 0))
{}

mmap_buffer& mmap_buffer::operator=(mmap_buffer&& other) noexcept
{
    if (this != &other) {
        if (buffer_ != nullptr) {
            munmap(buffer_, size_);
            buffer_ = nullptr;
        }
        buffer_ = std::exchange(other.buffer_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}

mmap_buffer::~mmap_buffer() noexcept
{
    if (buffer_ != nullptr) {
        munmap(buffer_, size_);
    }
    buffer_ = nullptr;
    size_ = 0;
}

std::expected<mmap_buffer, std::string> mmap_buffer::create(
    std::size_t size, mmap_flag flags) noexcept
{
    mmap_prot prot = mmap_prot::read | mmap_prot::write;
    flags |= mmap_flag::private_map | mmap_flag::populate;
    void* buffer = mmap(
        nullptr, size, static_cast<int>(prot), static_cast<int>(flags), -1, 0);
    if (buffer == MAP_FAILED) [[unlikely]] {
        auto flag_int = std::underlying_type_t<mmap_flag>(flags);
        auto&& exp_msg = mica::format(
            "mmap failed, error: {}, size: {}, flags: {}",
            std::strerror(errno), size, flag_int
        );
        if (!exp_msg.has_value()) [[unlikely]] {
            return std::unexpected("formatting failed while handling mmap error");
        }
        return std::unexpected(std::move(exp_msg).value());
    }
    return mmap_buffer(buffer, size);
}

std::expected<mmap_buffer, std::string> mmap_buffer::create(std::size_t size) noexcept
{
    mmap_flag flags = mmap_flag::private_map | mmap_flag::populate;
    return mmap_buffer::create(size, flags);
}

void* mmap_buffer::buffer() noexcept
{
    return buffer_;
}

const void* mmap_buffer::buffer() const noexcept
{
    return buffer_;
}

std::size_t mmap_buffer::size() const noexcept
{
    return size_;
}

} // namespace amber
