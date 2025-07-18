#include <amber/except/unexpected_error.hpp>

namespace amber {

unexpected_error::unexpected_error()
    : std::runtime_error("")
{}

} // namespace amber
