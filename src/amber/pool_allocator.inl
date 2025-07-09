#include <memory>
#include <utility>
#include <new>
#include <amber/except.hpp>

namespace amber {

template<typename T>
T* pool_allocator::allocate()
{
    if (sizeof(T) > entry_size) {
        throw invalid_allocate_error();
    }
    return std::launder(static_cast<T*>(allocate));
}

template<typename T, typename... Args>
T* pool_allocator::allocate_with_construct(Args&&... args)
{
    return std::launder(std::construct_at(allocate<T>(), std::forward<Args>(args)...));
}

template<typename T>
T* pool_allocator::try_allocate() noexcept
{
    if (sizeof(T) > entry_size) {
        return nullptr;
    }
    return std::launder(static_cast<T*>(try_allocate()));
}

template<typename T>
void pool_allocator::free(T* ptr)
{
    free(static_cast<void*>(ptr));
}

template<typename T>
void pool_allocator::free_with_destruct(T* ptr)
{
    std::destroy_at(ptr);
    free(static_cast<void*>(ptr));
}

template<typename T>
bool pool_allocator::try_free(T* ptr) noexcept
{
    return try_free(static_cast<void*>(ptr));
}

} // namespace amber
