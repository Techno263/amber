#include <amber/linear_allocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

namespace amber_test {

TEST_CASE("linear_allocator move constructor 1")
{
    amber::linear_allocator a1(64);
    REQUIRE(a1.size() == 64);
    REQUIRE(a1.offset() == 0);
    amber::linear_allocator a2(std::move(a1));
    REQUIRE(a1.size() == 0);
    REQUIRE(a1.offset() == 0);
    REQUIRE(a2.size() == 64);
    REQUIRE(a2.offset() == 0);
}

TEST_CASE("linear_allocator move constructor 2")
{
    amber::linear_allocator a1(64);
    REQUIRE(a1.size() == 64);
    REQUIRE(a1.offset() == 0);
    a1.allocate(24);
    REQUIRE(a1.size() == 64);
    REQUIRE(a1.offset() == 24);
    amber::linear_allocator a2 = std::move(a1);
    REQUIRE(a1.size() == 0);
    REQUIRE(a1.offset() == 0);
    REQUIRE(a2.size() == 64);
    REQUIRE(a2.offset() == 24);
}

TEST_CASE("linear_allocator param constructor(size)")
{
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);
}

TEST_CASE("linear_allocator param constructor(alignment, size)")
{
    amber::linear_allocator a1(64, 128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);
    std::uintptr_t p1 = reinterpret_cast<std::uintptr_t>(a1.allocate(30));
    REQUIRE((p1 % 64) == 0);
}

TEST_CASE("linear_allocator move assignment")
{
    amber::linear_allocator a1(64);
    REQUIRE(a1.size() == 64);
    REQUIRE(a1.offset() == 0);
    amber::linear_allocator a2 = std::move(a1);
    REQUIRE(a1.size() == 0);
    REQUIRE(a1.offset() == 0);
    REQUIRE(a2.size() == 64);
    REQUIRE(a2.offset() == 0);
}

TEST_CASE("linear_allocator allocate(alignment, size)")
{
    void* p = nullptr;
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    p = a1.allocate(1, 10);
    REQUIRE(a1.offset() == 10);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 1) == 0);

    p = a1.allocate(16, 20);
    REQUIRE(a1.offset() == 36);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 16) == 0);

    p = a1.allocate(4, 4);
    REQUIRE(a1.offset() == 40);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 4) == 0);
}

TEST_CASE("linear_allocator allocate(size)")
{
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    a1.allocate(13);
    REQUIRE(a1.offset() == 13);

    a1.allocate(13);
    REQUIRE(a1.offset() == 26);

    a1.allocate(37);
    REQUIRE(a1.offset() == 63);
}

TEST_CASE("linear_allocator allocate<T>(...)")
{
    int* int_ptr = nullptr;
    long* long_ptr = nullptr;
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    int_ptr = a1.allocate<int>(10);
    REQUIRE(int_ptr != nullptr);
    REQUIRE(a1.offset() == 4);
    REQUIRE((reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int)) == 0);

    long_ptr = a1.allocate<long>(11);
    REQUIRE(long_ptr != nullptr);
    REQUIRE(a1.offset() == 16);
    REQUIRE((reinterpret_cast<std::uintptr_t>(long_ptr) % alignof(long)) == 0);

    int_ptr = a1.allocate<int>(12);
    REQUIRE(int_ptr != nullptr);
    REQUIRE(a1.offset() == 20);
    REQUIRE((reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int)) == 0);
}

TEST_CASE("linear_allocator try_allocate(alignment, size)")
{
    void* p = nullptr;
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    p = a1.try_allocate(1, 10);
    REQUIRE(p != nullptr);
    REQUIRE(a1.offset() == 10);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 1) == 0);

    p = a1.try_allocate(16, 20);
    REQUIRE(p != nullptr);
    REQUIRE(a1.offset() == 36);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 16) == 0);

    p = a1.try_allocate(4, 4);
    REQUIRE(p != nullptr);
    REQUIRE(a1.offset() == 40);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 4) == 0);
}

TEST_CASE("linear_allocator try_allocate(size)")
{
    void* p = nullptr;
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    p = a1.try_allocate(13);
    REQUIRE(p != nullptr);
    REQUIRE(a1.offset() == 13);

    p = a1.try_allocate(13);
    REQUIRE(p != nullptr);
    REQUIRE(a1.offset() == 26);

    p = a1.try_allocate(37);
    REQUIRE(p != nullptr);
    REQUIRE(a1.offset() == 63);
}

TEST_CASE("linear_allocator try_allocate<T>(...)")
{
    int* int_ptr = nullptr;
    long* long_ptr = nullptr;
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    int_ptr = a1.try_allocate<int>(10);
    REQUIRE(int_ptr != nullptr);
    REQUIRE(a1.offset() == 4);
    REQUIRE((reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int)) == 0);

    long_ptr = a1.try_allocate<long>(11);
    REQUIRE(long_ptr != nullptr);
    REQUIRE(a1.offset() == 16);
    REQUIRE((reinterpret_cast<std::uintptr_t>(long_ptr) % alignof(long)) == 0);

    int_ptr = a1.try_allocate<int>(12);
    REQUIRE(int_ptr != nullptr);
    REQUIRE(a1.offset() == 20);
    REQUIRE((reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int)) == 0);
}

TEST_CASE("linear_allocator reset")
{
    amber::linear_allocator a1(128);
    REQUIRE(a1.size() == 128);
    REQUIRE(a1.offset() == 0);

    a1.reset();
    REQUIRE(a1.offset() == 0);

    a1.allocate(24);
    REQUIRE(a1.offset() == 24);
    a1.reset();
    REQUIRE(a1.offset() == 0);
}

} // namespace amber_test
