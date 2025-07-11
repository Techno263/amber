#pragma once

#include <stdexcept>

namespace amber {

class alignment_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    alignment_error();
};

} // namespace amber
