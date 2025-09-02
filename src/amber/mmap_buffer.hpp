#pragma once

extern "C" {
#include <sys/mman.h>
}
#include <amber/bitwise_enum.hpp>
#include <amber/concept.hpp>
#include <cstddef>
#include <expected>
#include <span>
#include <string>

namespace amber {

enum class mmap_flag : int {
    none = 0,
    private_map = MAP_PRIVATE,
    anonymous = MAP_ANONYMOUS,
    huge_table = MAP_HUGETLB,
    huge_2mb = MAP_HUGE_2MB,
    huge_1gb = MAP_HUGE_1GB,
    nonblock = MAP_NONBLOCK,
    no_reserve = MAP_NORESERVE,
    populate = MAP_POPULATE,
};
AMBER_BITWISE_ENUM(mmap_flag);

class mmap_buffer {
public:
    mmap_buffer() = delete;

    mmap_buffer(const mmap_buffer&) = delete;

    mmap_buffer(mmap_buffer&& other) noexcept;

    mmap_buffer& operator=(const mmap_buffer&) = delete;

    mmap_buffer& operator=(mmap_buffer&& other) noexcept;

    ~mmap_buffer() noexcept;

    static
    std::expected<mmap_buffer, std::string> create(
        std::size_t size, mmap_flag flags) noexcept;

    static
    std::expected<mmap_buffer, std::string> create(std::size_t size) noexcept;

    std::span<std::byte> buffer() noexcept;

    const std::span<std::byte> buffer() const noexcept;

    std::size_t size() const noexcept;

private:
    mmap_buffer(std::byte* buffer, std::size_t size) noexcept;

    std::byte* buffer_;
    std::size_t size_;
};

static_assert(Buffer<mmap_buffer>);

} // namespace amber
