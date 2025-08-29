#include <amber/mmap_buffer.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>

namespace amber_test {

TEST_CASE("mmap_buffer test")
{
    std::size_t buffer_size = 5200;
    auto exp_buffer = amber::mmap_buffer::create(buffer_size);
    if (!exp_buffer.has_value()) {
        UNSCOPED_INFO("mmap_buffer::create error: " << exp_buffer.error());
    }
    REQUIRE(exp_buffer.has_value());
    amber::mmap_buffer buffer(std::move(exp_buffer).value());
    REQUIRE(buffer.size() == buffer_size);
}

} // namespace amber_test
