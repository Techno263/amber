#pragma once

#include <stdexcept>

namespace amber {

class out_of_capacity_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    out_of_capacity_error();
};

} // namespace amber
