#include <memory>
#include <utility>
#include <new>

namespace amber {

template<typename T, typename... Args>
requires std::is_nothrow_constructible_v<T, Args...>
std::expected<T*, alloc_error> stack_allocator::allocate(Args&&... args) noexcept
{
    auto exp_ptr = allocate(alignof(T), sizeof(T));
    if (!exp_ptr.has_value()) [[unlikely]] {
        return std::unexpected(exp_ptr.error());
    }
    T* ptr = std::assume_aligned<alignof(T)>(static_cast<T*>(exp_ptr.value()));
    return std::launder(std::construct_at(ptr, std::forward<Args>(args)...));
}

template<typename T>
requires std::is_nothrow_destructible_v<T>
void stack_allocator::free(T* ptr) noexcept
{
    std::destroy_at(ptr);
    free(static_cast<void*>(ptr));
}

} // namespace amber
