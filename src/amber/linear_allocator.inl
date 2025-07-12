#include <memory>
#include <new>

namespace amber {

template<typename T, typename... Args>
requires std::is_trivially_destructible_v<T>
T* linear_allocator::allocate(Args&&... args)
{
    T* output = std::assume_aligned<alignof(T)>(static_cast<T*>(allocate(sizeof(T), alignof(T))));
    return std::launder(std::construct_at(output, std::forward<Args>(args)...));
}

template<typename T, typename... Args>
requires std::is_trivially_destructible_v<T> && std::is_nothrow_constructible_v<T, Args...>
T* linear_allocator::try_allocate(Args&&... args) noexcept
{
    T* output = std::assume_aligned<alignof(T)>(static_cast<T*>(allocate(sizeof(T), alignof(T))));
    return std::launder(std::construct_at(output, std::forward<Args>(args)...));
}

}
