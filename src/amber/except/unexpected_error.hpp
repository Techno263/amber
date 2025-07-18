#pragma once

#include <stdexcept>

namespace amber {

class unexpected_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    unexpected_error();
};

} // namespace amber
