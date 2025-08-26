#pragma once

#include <amber/concept.hpp>
#include <cstddef>
#include <expected>
#include <span>
#include <string>

namespace amber {

class malloc_buffer {
public:
    malloc_buffer() = delete;

    malloc_buffer(const malloc_buffer&) = delete;

    malloc_buffer(malloc_buffer&& other) noexcept;

    malloc_buffer& operator=(const malloc_buffer&) = delete;

    malloc_buffer& operator=(malloc_buffer&& other) noexcept;

    ~malloc_buffer() noexcept;

    static
    std::expected<malloc_buffer, std::string> create(std::size_t size) noexcept;

    std::span<std::byte> buffer() noexcept;

    const std::span<std::byte> buffer() const noexcept;

    std::size_t size() const noexcept;

private:
    malloc_buffer(std::byte* buffer, std::size_t size) noexcept;

    std::byte* buffer_;
    std::size_t size_;
};

static_assert(Buffer<malloc_buffer>);

} // namespace amber
