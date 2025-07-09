#include <amber/except.hpp>

namespace amber {

template<typename IntegerType>
requires std::is_integral_v<IntegralType>
constexpr
bool is_power_of_two(IntegerType n) noexcept
{
    return n != 0 && (n & (n - 1)) == 0;
}

template<typename IntegerType>
requires std::is_integral_v<IntegerType>
constexpr IntegerType align_forward(IntegerType num, IntegerType align)
{
    if (!is_power_of_two(align)) {
        throw alignment_error("align must be a power of two");
    }
    return align_forward_no_check(num, align);
}

template<typename IntegerType>
requires std::is_integral_v<IntegerType>
constexpr IntegerType align_forward_no_check(IntegerType num, IntegerType align) noexcept
{
    IntegerType modulo = num & (align - 1);
    if (modulo != 0) {
        num += align - modulo;
    }
    return num;
}

} // namespace amber
