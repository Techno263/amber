#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace amber {

template<typename T>
concept Buffer =
    !std::is_copy_constructible_v<T>
    && !std::is_copy_assignable_v<T>
    && std::is_nothrow_move_constructible_v<T>
    && std::is_nothrow_move_assignable_v<T>
    && requires(T t, const T tc)
{
    { t.buffer() } noexcept -> std::same_as<void*>;
    { tc.buffer() } noexcept -> std::same_as<const void*>;
    { tc.size() } noexcept -> std::same_as<std::size_t>;
};

} // namespace amber
