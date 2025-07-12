#include <amber/except.hpp>
#include <memory>
#include <utility>
#include <new>

namespace amber {

template<typename T, typename... Args>
T* pool_allocator::allocate(Args&&... args)
{
    if (sizeof(T) > entry_size) {
        throw invalid_allocate_error();
    }
    T* output = std::assume_aligned<alignof(T)>(static_cast<T*>(allocate()));
    return std::launder(std::construct_at(output, std::forward<Args>(args)...));
}

template<typename T, typename... Args>
requires std::is_nothrow_constructible_v<T, Args...>
T* pool_allocator::try_allocate(Args&&... args) noexcept
{
    if (sizeof(T) > entry_size) {
        return nullptr;
    }
    T* output = std::assume_aligned<alignof(T)>(static_cast<T*>(allocate()));
    return std::launder(std::construct_at(output, std::forward<Args>(args)...));
}

template<typename T>
void pool_allocator::free(T* ptr)
{
    std::destroy_at(ptr);
    free(static_cast<void*>(ptr));
}

template<typename T>
requires std::is_nothrow_destructible_v<T>
bool pool_allocator::try_free(T* ptr) noexcept
{
    std::destroy_at(ptr);
    return try_free(static_cast<void*>(ptr));
}

} // namespace amber
