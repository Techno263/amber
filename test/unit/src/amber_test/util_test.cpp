#include <amber/util.hpp>
#include <array>
#include <bit>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstring>

namespace amber_test {

TEST_CASE("is_aligned")
{
    struct test_case {
        struct {
            std::size_t alignment;
            std::size_t value;
        } input;
        std::size_t expect;
    };
    std::array test_cases{
        test_case{
            .input = {.alignment = 1, .value = 1},
            .expect = true
        },
        test_case{
            .input = {.alignment = 1, .value = 2},
            .expect = true
        },
        test_case{
            .input = {.alignment = 2, .value = 2},
            .expect = true
        },
        test_case{
            .input = {.alignment = 2, .value = 102},
            .expect = true
        },
        test_case{
            .input = {.alignment = 4, .value = 16},
            .expect = true
        },
        test_case{
            .input = {.alignment = 4, .value = 128},
            .expect = true
        },
        test_case{
            .input = {.alignment = 8, .value = 16},
            .expect = true
        },
        test_case{
            .input = {.alignment = 8, .value = 96},
            .expect = true
        },
        test_case{
            .input = {.alignment = 16, .value = 64},
            .expect = true
        },
        test_case{
            .input = {.alignment = 128, .value = 1024},
            .expect = true
        },
        test_case{
            .input = {.alignment = 2, .value = 3},
            .expect = false
        },
        test_case{
            .input = {.alignment = 16, .value = 24},
            .expect = false
        },
        test_case{
            .input = {.alignment = 16, .value = 33},
            .expect = false
        },
        test_case{
            .input = {.alignment = 8, .value = 14},
            .expect = false
        },
    };
    for (const test_case& tc : test_cases) {
        std::size_t actual = amber::is_aligned(tc.input.alignment, tc.input.value);
        REQUIRE(actual == tc.expect);
    }
}

TEST_CASE("align_forward true")
{
    struct test_case {
        struct {
            std::size_t num;
            std::size_t alignment;
        } input;
        std::size_t expect;
    };
    std::array test_cases{
        test_case{
            .input = {.num = 0, .alignment = 1},
            .expect = 0
        },
        test_case{
            .input = {.num = 1, .alignment = 1},
            .expect = 1
        },
        test_case{
            .input = {.num = 0, .alignment = 1},
            .expect = 0
        },
        test_case{
            .input = {.num = 2, .alignment = 1},
            .expect = 2
        },
        test_case{
            .input = {.num = 1, .alignment = 2},
            .expect = 2
        },
        test_case{
            .input = {.num = 2, .alignment = 2},
            .expect = 2
        },
        test_case{
            .input = {.num = 3, .alignment = 2},
            .expect = 4
        },
        test_case{
            .input = {.num = 4, .alignment = 2},
            .expect = 4
        },
        test_case{
            .input = {.num = 20, .alignment = 8},
            .expect = 24
        },
        test_case{
            .input = {.num = 21, .alignment = 8},
            .expect = 24
        },
        test_case{
            .input = {.num = 24, .alignment = 8},
            .expect = 24
        },
        test_case{
            .input = {.num = 25, .alignment = 8},
            .expect = 32
        },
        test_case{
            // num = 2^60 - 1, alignment = 2^60
            .input = {.num = 1152921504606846975, .alignment = 1152921504606846976},
            // expect = 2*60
            .expect = 1152921504606846976
        },
        test_case{
            // num = 2^60, alignment = 2^60
            .input = {.num = 1152921504606846976, .alignment = 1152921504606846976},
            // expect = 2*60
            .expect = 1152921504606846976
        },
        test_case{
            // num = 2^60 + 1, alignment = 2^60
            .input = {.num = 1152921504606846977, .alignment = 1152921504606846976},
            // expect = 2^60 * 2
            .expect = 2305843009213693952
        }
    };
    for (const test_case& tc : test_cases) {
        UNSCOPED_INFO("num: " << tc.input.num);
        UNSCOPED_INFO("alignment: " << tc.input.alignment);
        std::size_t actual = amber::align_forward(tc.input.alignment, tc.input.num);
        UNSCOPED_INFO("expect: " << tc.expect);
        UNSCOPED_INFO("actual: " << actual);
        REQUIRE(actual == tc.expect);
    }
}

TEST_CASE("align_forward alignment_error")
{
    struct test_case {
        struct {
            std::size_t num;
            std::size_t alignment;
        } input;
    };
    std::array test_cases{
        test_case{
            .input = {.num = 0, .alignment = 0},
        },
        test_case{
            .input = {.num = 0, .alignment = 3},
        },
        test_case{
            .input = {.num = 0, .alignment = 24},
        }
    };
    for (const test_case& tc : test_cases) {
        UNSCOPED_INFO("num: " << tc.input.num);
        UNSCOPED_INFO("alignment: " << tc.input.alignment);
        std::size_t actual = amber::align_forward<std::size_t>(tc.input.alignment, tc.input.num);
        REQUIRE(actual == 0);
    }
}

} // namespace amber_test
