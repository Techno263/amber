#include <amber/except/invalid_allocate_error.hpp>

namespace amber {

invalid_allocate_error::invalid_allocate_error()
    : std::runtime_error("")
{}

} // namespace amber
