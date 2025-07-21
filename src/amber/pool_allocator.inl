#include <memory>
#include <utility>
#include <new>

namespace amber {

template<typename T, typename... Args>
std::expected<T*, allocate_error> pool_allocator::allocate(Args&&... args) noexcept
{
    if (sizeof(T) > entry_size_) [[unlikely]] {
        return std::unexpected(allocate_error::invalid_alloc_error);
    }
    auto exp_ptr = allocate();
    if (!exp_ptr.has_value()) [[unlikely]] {
        return std::unexpected(exp_ptr.error());
    }
    T* ptr = std::assume_aligned<alignof(T)>(static_cast<T*>(exp_ptr.value()));
    return std::launder(std::construct_at(ptr, std::forward<Args>(args)...));
}

template<typename T>
void pool_allocator::free(T* ptr) noexcept
{
    std::destroy_at(ptr);
    free(static_cast<void*>(ptr));
}

} // namespace amber
