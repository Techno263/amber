#include <amber/except.hpp>
#include <amber/util.hpp>
#include <array>
#include <bit>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstring>

namespace amber_test {

TEST_CASE("align_forward true")
{
    struct test_case {
        struct {
            std::size_t num;
            std::size_t align;
        } input;
        std::size_t expect;
    };
    std::array test_cases{
        test_case{
            .input = {.num = 0, .align = 1},
            .expect = 0
        },
        test_case{
            .input = {.num = 1, .align = 1},
            .expect = 1
        },
        test_case{
            .input = {.num = 0, .align = 1},
            .expect = 0
        },
        test_case{
            .input = {.num = 2, .align = 1},
            .expect = 2
        },
        test_case{
            .input = {.num = 1, .align = 2},
            .expect = 2
        },
        test_case{
            .input = {.num = 2, .align = 2},
            .expect = 2
        },
        test_case{
            .input = {.num = 3, .align = 2},
            .expect = 4
        },
        test_case{
            .input = {.num = 4, .align = 2},
            .expect = 4
        },
        test_case{
            .input = {.num = 20, .align = 8},
            .expect = 24
        },
        test_case{
            .input = {.num = 21, .align = 8},
            .expect = 24
        },
        test_case{
            .input = {.num = 24, .align = 8},
            .expect = 24
        },
        test_case{
            .input = {.num = 25, .align = 8},
            .expect = 32
        },
        test_case{
            // num = 2^60 - 1, align = 2^60
            .input = {.num = 1152921504606846975, .align = 1152921504606846976},
            // expect = 2*60
            .expect = 1152921504606846976
        },
        test_case{
            // num = 2^60, align = 2^60
            .input = {.num = 1152921504606846976, .align = 1152921504606846976},
            // expect = 2*60
            .expect = 1152921504606846976
        },
        test_case{
            // num = 2^60 + 1, align = 2^60
            .input = {.num = 1152921504606846977, .align = 1152921504606846976},
            // expect = 2^60 * 2
            .expect = 2305843009213693952
        }
    };
    for (const test_case& tc : test_cases) {
        UNSCOPED_INFO("num: " << tc.input.num);
        UNSCOPED_INFO("align: " << tc.input.align);
        std::size_t actual = amber::align_forward(tc.input.num, tc.input.align);
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
            std::size_t align;
        } input;
        const char* expect_err;
    };
    std::array test_cases{
        test_case{
            .input = {.num = 0, .align = 0},
            .expect_err = "align must be a power of two"
        },
        test_case{
            .input = {.num = 0, .align = 3},
            .expect_err = "align must be a power of two"
        },
        test_case{
            .input = {.num = 0, .align = 24},
            .expect_err = "align must be a power of two"
        }
    };
    for (const test_case& tc : test_cases) {
        UNSCOPED_INFO("num: " << tc.input.num);
        UNSCOPED_INFO("align: " << tc.input.align);
        try {
            std::size_t actual = amber::align_forward<std::size_t>(tc.input.num, tc.input.align);
            UNSCOPED_INFO("Unexpected successful function call, actual: " << actual);
            REQUIRE(false);
        } catch (const amber::alignment_error& e) {
            const char* actual_err = e.what();
            UNSCOPED_INFO("expect_err: " << tc.expect_err);
            UNSCOPED_INFO("actual_err: " << actual_err);
            REQUIRE(std::strcmp(e.what(), tc.expect_err) == 0);
        } catch (...) {
            UNSCOPED_INFO("Unexpected exception");
            REQUIRE(false);
        }
    }
}

} // namespace amber_test
