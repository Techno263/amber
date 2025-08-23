#pragma once

#include <type_traits>

// Adds bitwise operators to a class enum
// After declaring a class enum put `BITWISE_ENUM(<enum-name>);` to use
#define AMBER_BITWISE_ENUM(enum_type_) \
inline constexpr enum_type_ operator~(const enum_type_& a) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    return static_cast<enum_type_>(~static_cast<T>(a)); \
} \
inline constexpr enum_type_ operator&(const enum_type_& a, const enum_type_& b) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    return static_cast<enum_type_>(static_cast<T>(a) & static_cast<T>(b)); \
} \
inline constexpr enum_type_ operator|(const enum_type_& a, const enum_type_& b) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    return static_cast<enum_type_>(static_cast<T>(a) | static_cast<T>(b)); \
} \
inline constexpr enum_type_ operator^(const enum_type_& a, const enum_type_& b) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    return static_cast<enum_type_>(static_cast<T>(a) ^ static_cast<T>(b)); \
} \
inline constexpr enum_type_& operator&=(enum_type_& a, const enum_type_& b) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    a = static_cast<enum_type_>(static_cast<T>(a) & static_cast<T>(b)); \
    return a; \
} \
inline constexpr enum_type_& operator|=(enum_type_& a, const enum_type_& b) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    a = static_cast<enum_type_>(static_cast<T>(a) | static_cast<T>(b)); \
    return a; \
} \
inline constexpr enum_type_& operator^=(enum_type_& a, const enum_type_& b) \
{ \
    using T = std::underlying_type_t<enum_type_>; \
    a = static_cast<enum_type_>(static_cast<T>(a) ^ static_cast<T>(b)); \
    return a; \
}
