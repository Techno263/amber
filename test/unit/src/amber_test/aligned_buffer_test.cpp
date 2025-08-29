#include <amber/aligned_buffer.hpp>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>

namespace amber_test {

TEST_CASE("aligned_buffer test")
{
    std::array<std::size_t, 12> sizes{{
        1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
    }};

    std::size_t buffer_size = 4096;
    for (std::size_t target_alignment : sizes) {
        auto exp_buffer = amber::aligned_buffer::create(target_alignment, buffer_size);
        if (!exp_buffer.has_value()) {
            UNSCOPED_INFO("aligned_buffer::create error: " << exp_buffer.error());
        }
        REQUIRE(exp_buffer.has_value());
        amber::aligned_buffer buffer(std::move(exp_buffer).value());
        REQUIRE((buffer.alignment() % target_alignment) == 0);
        REQUIRE(buffer.size() == buffer_size);
    }
}

} // namespace amber_test
