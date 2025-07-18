#include <amber/except.hpp>
#include <amber/util.hpp>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>

namespace amber {

namespace {

enum class malloc_status {
    success,
    alignment_error,
    bad_alloc,
};

struct malloc_result {
public:
    malloc_result(void* ptr, malloc_status status) noexcept
        : ptr(ptr), status(status)
    {}

    void* ptr;
    malloc_status status;
};

inline malloc_result _aligned_malloc_no_check_helper(std::size_t alignment, std::size_t size) noexcept
{
    // Sanity checks
    static_assert((sizeof(void*) % alignof(void*)) == 0);
    static_assert(sizeof(void*) == sizeof(std::byte*));
    static_assert(alignof(void*) == alignof(std::byte*));
    static_assert(sizeof(void*) <= alignof(std::max_align_t));
    static_assert((alignof(std::max_align_t) % alignof(void*)) == 0);

    if (alignment < alignof(std::max_align_t)) {
        alignment = alignof(std::max_align_t);
    }
    std::byte* malloc_ptr = static_cast<std::byte*>(std::malloc(alignment + size));
    if (malloc_ptr == nullptr) {
        return malloc_result(nullptr, malloc_status::bad_alloc);
    }
    std::byte* shifted_ptr = malloc_ptr + alignof(std::max_align_t);
    std::uintptr_t shifted_addr = reinterpret_cast<std::uintptr_t>(shifted_ptr);
    std::uintptr_t aligned_addr = align_forward_no_check(alignment, shifted_addr);
    std::byte* aligned_ptr = reinterpret_cast<std::byte*>(aligned_addr);
    aligned_ptr = std::assume_aligned<alignof(std::max_align_t)>(aligned_ptr);
    std::memcpy(aligned_ptr - sizeof(void*), &malloc_ptr, sizeof(void*));
    return malloc_result(static_cast<void*>(aligned_ptr), malloc_status::success);
}

inline malloc_result _aligned_malloc_helper(std::size_t alignment, std::size_t size) noexcept
{
    if (!std::has_single_bit(alignment)) {
        return malloc_result(nullptr, malloc_status::alignment_error);
    }
    return _aligned_malloc_no_check_helper(alignment, size);
}

inline void* _aligned_malloc_no_check(std::size_t alignment, std::size_t size)
{
    malloc_result res = _aligned_malloc_no_check_helper(alignment, size);
    switch (res.status) {
    case malloc_status::success:
        return res.ptr;
    case malloc_status::bad_alloc:
        throw std::bad_alloc();
    default:
        throw unexpected_error();
    }
}

inline void* _aligned_malloc(std::size_t alignment, std::size_t size)
{
    malloc_result res = _aligned_malloc_helper(alignment, size);
    switch (res.status) {
    case malloc_status::success:
        return res.ptr;
    case malloc_status::alignment_error:
        throw alignment_error("alignment must be a power of two");
    case malloc_status::bad_alloc:
        throw std::bad_alloc();
    default:
        throw unexpected_error();
    }
}

inline void* _try_aligned_malloc_no_check(std::size_t alignment, std::size_t size) noexcept
{
    malloc_result res = _aligned_malloc_no_check_helper(alignment, size);
    switch (res.status) {
    case malloc_status::success:
        return res.ptr;
    default:
        return nullptr;
    }
}

inline void* _try_aligned_malloc(std::size_t alignment, std::size_t size) noexcept
{
    malloc_result res = _aligned_malloc_helper(alignment, size);
    switch (res.status) {
    case malloc_status::success:
        return res.ptr;
    default:
        return nullptr;
    }
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

void* aligned_malloc_no_check(std::size_t alignment, std::size_t size)
{
#ifdef AMBER_USE_CPP_ALIGNED_ALLOC
    return std::aligned_alloc(alignment, size);
#else
    return _aligned_malloc(alignment, size);
#endif
}

void* aligned_malloc(std::size_t alignment, std::size_t size)
{
#ifdef AMBER_USE_CPP_ALIGNED_ALLOC
    return std::aligned_alloc(alignment, size);
#else
    return _aligned_malloc(alignment, size);
#endif
}

void* try_aligned_malloc_no_check(std::size_t alignment, std::size_t size) noexcept
{
#ifdef AMBER_USE_CPP_ALIGNED_ALLOC
    return std::aligned_alloc(alignment, size);
#else
    return _try_aligned_malloc_no_check(alignment, size);
#endif
}

void* try_aligned_malloc(std::size_t alignment, std::size_t size) noexcept
{
#ifdef AMBER_USE_CPP_ALIGNED_ALLOC
    return std::aligned_alloc(alignment, size);
#else
    return _try_aligned_malloc(alignment, size);
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
