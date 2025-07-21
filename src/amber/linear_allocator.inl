#include <memory>
#include <new>

namespace amber {

template<typename T, typename... Args>
requires std::is_trivially_destructible_v<T>
std::expected<T*, alloc_error> linear_allocator::allocate(Args&&... args) noexcept
{
    auto exp_ptr = allocate(alignof(T), sizeof(T));
    if (!exp_ptr.has_value()) [[unlikely]] {
        return std::unexpected(exp_ptr.error());
    }
    T* ptr = std::assume_aligned<alignof(T)>(static_cast<T*>(exp_ptr.value()));
    return std::launder(std::construct_at(ptr, std::forward<Args>(args)...));
}

}
