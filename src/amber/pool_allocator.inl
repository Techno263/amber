#include <amber/util.hpp>
#include <algorithm>
#include <memory>
#include <new>

namespace amber {

template<Buffer B>
std::expected<pool_allocator, std::string> pool_allocator::create(
    B& buffer, std::size_t entry_size) noexcept
{
    std::size_t buffer_size = buffer.size();
    entry_size = std::max(entry_size, sizeof(internal::pool_entry));
    if ((entry_size & (alignof(internal::pool_entry) - 1)) == 0) {
        entry_size = align_forward(alignof(internal::pool_entry), entry_size);
    }
    std::size_t entry_count = buffer_size / entry_size;
    
    std::byte* buffer_ptr = static_cast<std::byte*>(buffer.buffer());
    std::byte* free_head = nullptr;
    static_assert(std::is_nothrow_constructible_v<internal::pool_entry, decltype(free_head)>);
    for (std::size_t i = 0; i < entry_count; ++i) {
        internal::pool_entry* entry_ptr = reinterpret_cast<internal::pool_entry*>(buffer_ptr + (i * entry_size));
        entry_ptr = std::assume_aligned<alignof(internal::pool_entry)>(entry_ptr);
        entry_ptr = std::launder(std::construct_at(entry_ptr, free_head));
        free_head = reinterpret_cast<std::byte*>(entry_ptr);
    }

    return pool_allocator(buffer_ptr, free_head, buffer_size, entry_size, entry_count, 0);
}

template<typename T, typename... Args>
requires std::is_nothrow_constructible_v<T, Args...>
std::expected<T*, std::string> pool_allocator::allocate(Args&&... args) noexcept
{
    if (sizeof(T) > entry_size_) [[unlikely]] {
        return std::unexpected("type size too large");
    }
    auto exp_ptr = allocate();
    if (!exp_ptr.has_value()) [[unlikely]] {
        return std::unexpected(std::move(exp_ptr).error());
    }
    T* ptr = std::assume_aligned<alignof(T)>(static_cast<T*>(std::move(exp_ptr).value()));
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
