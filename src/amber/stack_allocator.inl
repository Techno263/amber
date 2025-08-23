#include <amber/util.hpp>
#include <cstdint>
#include <memory>
#include <mica/mica.hpp>
#include <new>
#include <utility>

namespace amber {

template<Buffer B>
std::expected<stack_allocator, std::string> stack_allocator::create(B& buffer) noexcept
{
    std::byte* buffer_ptr = static_cast<std::byte*>(buffer.buffer());
    std::uintptr_t buffer_addr = reinterpret_cast<std::uintptr_t>(buffer_ptr);
    std::uintptr_t target_alignment = static_cast<std::uintptr_t>(alignof(std::max_align_t));
    if (!is_aligned(target_alignment, buffer_addr)) [[unlikely]] {
        auto&& exp_msg = mica::format(
            "invalid buffer alignment: buffer: {}, target alignment: {}",
            buffer.buffer(), target_alignment
        );
        if (!exp_msg.has_value()) [[unlikely]] {
            return std::unexpected("formatting failed while handling alignment error");
        }
        return std::unexpected(std::move(exp_msg).value());
    }
    return stack_allocator(buffer.buffer(), buffer.size(), 0);
}

template<typename T, typename... Args>
requires std::is_nothrow_constructible_v<T, Args...>
std::expected<T*, std::string> stack_allocator::allocate(Args&&... args) noexcept
{
    auto exp_ptr = allocate(alignof(T), sizeof(T));
    if (!exp_ptr.has_value()) [[unlikely]] {
        return exp_ptr;
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
