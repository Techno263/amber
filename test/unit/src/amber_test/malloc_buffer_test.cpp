#include <amber/malloc_buffer.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>

namespace amber_test {

TEST_CASE("malloc_buffer test")
{
    std::size_t buffer_size = 999;
    auto exp_buffer = amber::malloc_buffer::create(buffer_size);
    if (!exp_buffer.has_value()) {
        UNSCOPED_INFO("malloc_buffer::create error: " << exp_buffer.error());
    }
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer(std::move(exp_buffer).value());
    REQUIRE(buffer.size() == buffer_size);
}

} // namespace amber_test
