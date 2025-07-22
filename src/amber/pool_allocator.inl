#include <memory>
#include <utility>
#include <new>

namespace amber {

template<typename T>
std::expected<pool_allocator, alloc_error> pool_allocator::create(
    std::size_t entry_count) noexcept
{
    // Always directly return `create` for RVO
    return create(
        std::max(alignof(T), alignof(std::max_align_t)),
        alignof(T),
        sizeof(T),
        entry_count
    );
}

template<typename T, typename... Args>
requires std::is_nothrow_constructible_v<T, Args...>
std::expected<T*, alloc_error> pool_allocator::allocate(Args&&... args) noexcept
{
    if (sizeof(T) > entry_size_) [[unlikely]] {
        return std::unexpected(alloc_error::invalid_alloc_error);
    }
    auto exp_ptr = allocate();
    if (!exp_ptr.has_value()) [[unlikely]] {
        return std::unexpected(exp_ptr.error());
    }
    T* ptr = std::assume_aligned<alignof(T)>(static_cast<T*>(exp_ptr.value()));
    return std::launder(std::construct_at(ptr, std::forward<Args>(args)...));
}

template<typename T>
requires std::is_nothrow_destructible_v<T>
void pool_allocator::free(T* ptr) noexcept
{
    std::destroy_at(ptr);
    free(static_cast<void*>(ptr));
}

} // namespace amber
