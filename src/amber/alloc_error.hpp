#pragma once

namespace amber {

enum class alloc_error {
    alignment_error,
    bad_alloc_error,
    invalid_alloc_error,
    out_of_capacity_error,
};

} // namespace amber
