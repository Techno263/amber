#pragma once

#include <stdexcept>

namespace amber {

class invalid_allocate : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace amber
