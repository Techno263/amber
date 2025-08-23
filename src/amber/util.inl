#include <bit>
#include <utility>

namespace amber {

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr bool is_aligned(IntegerType alignment, IntegerType value) noexcept
{
    if (!std::has_single_bit(alignment)) {
        return false;
    }
    return ((alignment - 1) & value) == 0;
}

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType alignment, IntegerType value) noexcept
{
    if (!std::has_single_bit(alignment)) {
        return 0;
    }
    // based of gcc's libstdc++ align function
    return (value - 1 + alignment) & -alignment;
}

} // namespace amber
