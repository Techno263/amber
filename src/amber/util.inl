#include <amber/except.hpp>
#include <bit>

namespace amber {

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType is_aligned(IntegerType alignment, IntegerType value)
{
    if (!std::has_single_bit(alignment)) {
        throw alignment_error("alignment must be a power of two");
    }
    return is_aligned_no_check(alignment, value);
}

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType is_aligned_no_check(IntegerType alignment, IntegerType value) noexcept
{
    return ((alignment - 1) & value) == 0;
}

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType alignment, IntegerType value)
{
    if (!std::has_single_bit(alignment)) {
        throw alignment_error("alignment must be a power of two");
    }
    return align_forward_no_check(alignment, value);
}

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward_no_check(IntegerType alignment, IntegerType value) noexcept
{
    // based of gcc's libstdc++ align function
    return (value - 1 + alignment) & -alignment;
}

} // namespace amber
