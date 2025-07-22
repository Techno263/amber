#include <amber/util.hpp>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <memory>
#include <new>

namespace amber {

namespace {

inline std::expected<void*, alloc_error> _aligned_alloc(std::size_t alignment, std::size_t size) noexcept
{
    // Sanity checks
    static_assert((sizeof(void*) % alignof(void*)) == 0);
    static_assert(sizeof(void*) == sizeof(std::byte*));
    static_assert(alignof(void*) == alignof(std::byte*));
    static_assert(sizeof(void*) <= alignof(std::max_align_t));
    static_assert((alignof(std::max_align_t) % alignof(void*)) == 0);

    if (!std::has_single_bit(alignment)) {
        return std::unexpected(alloc_error::alignment_error);
    }
    if (alignment < alignof(std::max_align_t)) {
        alignment = alignof(std::max_align_t);
    }
    std::byte* malloc_ptr = static_cast<std::byte*>(std::malloc(alignment + size));
    if (malloc_ptr == nullptr) {
        return std::unexpected(alloc_error::bad_alloc_error);
    }
    std::byte* shifted_ptr = malloc_ptr + alignof(std::max_align_t);
    std::uintptr_t shifted_addr = reinterpret_cast<std::uintptr_t>(shifted_ptr);
    std::uintptr_t aligned_addr = align_forward(alignment, shifted_addr);
    std::byte* aligned_ptr = reinterpret_cast<std::byte*>(aligned_addr);
    aligned_ptr = std::assume_aligned<alignof(std::max_align_t)>(aligned_ptr);
    std::memcpy(aligned_ptr - sizeof(void*), &malloc_ptr, sizeof(void*));
    return static_cast<void*>(aligned_ptr);
}

inline void _aligned_free(void* ptr) noexcept
{
    if (ptr == nullptr) {
        return;
    }
    std::byte* aligned_ptr = static_cast<std::byte*>(ptr);
    std::byte* orig_ptr = nullptr;
    std::memcpy(&orig_ptr, aligned_ptr - sizeof(void*), sizeof(void*));
    std::free(static_cast<void*>(orig_ptr));
}

} // unnamed namespace

std::expected<void*, alloc_error> aligned_alloc(std::size_t alignment, std::size_t size) noexcept
{
#ifdef AMBER_USE_CPP_ALIGNED_ALLOC
    return std::aligned_alloc(alignment, size);
#else
    return _aligned_alloc(alignment, size);
#endif
}

void aligned_free(void* ptr) noexcept
{
#ifdef AMBER_USE_CPP_ALIGNED_ALLOC
    std::free(ptr);
#else
    _aligned_free(ptr);
#endif
}

} // namespace amber
