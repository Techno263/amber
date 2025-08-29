#pragma once

#include <amber/concept.hpp>
#include <cstddef>
#include <expected>
#include <span>
#include <string>

namespace amber {

class aligned_buffer {
public:
    aligned_buffer() = delete;

    aligned_buffer(const aligned_buffer&) = delete;

    aligned_buffer(aligned_buffer&& other) noexcept;

    aligned_buffer& operator=(const aligned_buffer&) = delete;

    aligned_buffer& operator=(aligned_buffer&& other) noexcept;

    ~aligned_buffer() noexcept;

    static
    std::expected<aligned_buffer, std::string> create(
        std::size_t alignment, std::size_t size) noexcept;

    std::span<std::byte> buffer() noexcept;

    const std::span<std::byte> buffer() const noexcept;

    std::size_t alignment() const noexcept;

    std::size_t size() const noexcept;

private:
    aligned_buffer(std::byte* buffer, std::size_t alignment, std::size_t size) noexcept;

    std::byte* buffer_;
    std::size_t alignment_;
    std::size_t size_;
};

static_assert(Buffer<aligned_buffer>);

} // namespace amber
