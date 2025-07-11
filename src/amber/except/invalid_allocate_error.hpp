#pragma once

#include <stdexcept>

namespace amber {

class invalid_allocate_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    invalid_allocate_error();
};

} // namespace amber
