#include <amber/except/out_of_capacity_error.hpp>

namespace amber {

out_of_capacity_error::out_of_capacity_error()
    : std::runtime_error("")
{}

} // namespace amber
