#include <amber/util.hpp>
#include <format>
#include <memory>
#include <memory>
#include <mica/mica.hpp>
#include <new>
#include <utility>

namespace amber {

template<Buffer B>
std::expected<linear_allocator, std::string> linear_allocator::create(B& buffer) noexcept
{
    std::byte* buffer_ptr = static_cast<std::byte*>(buffer.buffer());
    std::uintptr_t buffer_addr = reinterpret_cast<std::uintptr_t>(buffer_ptr);
    std::uintptr_t target_alignment = static_cast<std::uintptr_t>(alignof(std::max_align_t));
    if (!is_aligned(target_alignment, buffer_addr)) [[unlikely]] {
        auto&& exp_msg = mica::format(
            "invalid buffer alignment, buffer: {:#x}, target alignment: {}",
            buffer_addr, alignof(std::max_align_t)
        );
        if (!exp_msg.has_value()) [[unlikely]] {
            return std::unexpected("formatting failed while handling alignment error");
        }
        return std::unexpected(std::move(exp_msg).value());
    }
    return linear_allocator(buffer_ptr, buffer.size(), 0);
}

template<typename T, typename... Args>
requires std::is_trivially_destructible_v<T>
std::expected<T*, std::string> linear_allocator::allocate(Args&&... args) noexcept
{
    auto exp_ptr = allocate(alignof(T), sizeof(T));
    if (!exp_ptr.has_value()) [[unlikely]] {
        return std::unexpected(std::move(exp_ptr).error());
    }
    T* ptr = std::assume_aligned<alignof(T)>(static_cast<T*>(exp_ptr.value()));
    return std::launder(std::construct_at(ptr, std::forward<Args>(args)...));
}

} // namespace amber
