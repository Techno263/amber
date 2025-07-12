#include <amber/except.hpp>
#include <bit>

namespace amber {

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward(IntegerType num, IntegerType align)
{
    if (!std::has_single_bit(align)) {
        throw alignment_error("align must be a power of two");
    }
    return align_forward_no_check(num, align);
}

template<typename IntegerType>
requires std::unsigned_integral<IntegerType>
constexpr IntegerType align_forward_no_check(IntegerType num, IntegerType align) noexcept
{
    // based of gcc's libstdc++ align function
    return (num - 1 + align) & -align;
}

} // namespace amber
