#include <amber/alloc_error.hpp>
#include <amber/linear_allocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <expected>

namespace amber_test {

TEST_CASE("linear_allocator move constructor")
{
    auto exp_a1 = amber::linear_allocator::create(64);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 64);
    REQUIRE(a1.buffer_offset() == 0);
    amber::linear_allocator a2(std::move(a1));
    REQUIRE(a1.buffer_size() == 0);
    REQUIRE(a1.buffer_offset() == 0);
    REQUIRE(a2.buffer_size() == 64);
    REQUIRE(a2.buffer_offset() == 0);
}

TEST_CASE("linear_allocator move assignment")
{
    auto exp_a1 = amber::linear_allocator::create(64);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1 = std::move(exp_a1.value());
    REQUIRE(a1.buffer_size() == 64);
    REQUIRE(a1.buffer_offset() == 0);
    amber::linear_allocator a2 = std::move(a1);
    REQUIRE(a1.buffer_size() == 0);
    REQUIRE(a1.buffer_offset() == 0);
    REQUIRE(a2.buffer_size() == 64);
    REQUIRE(a2.buffer_offset() == 0);

    a2.allocate(24);
    REQUIRE(a2.buffer_size() == 64);
    REQUIRE(a2.buffer_offset() == 24);
    amber::linear_allocator a3 = std::move(a2);
    REQUIRE(a2.buffer_size() == 0);
    REQUIRE(a2.buffer_offset() == 0);
    REQUIRE(a3.buffer_size() == 64);
    REQUIRE(a3.buffer_offset() == 24);
}

TEST_CASE("linear_allocator create(size)")
{
    auto exp_a1 = amber::linear_allocator::create(128);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);
}

TEST_CASE("linear_allocator create(alignement, size)")
{
    auto exp_a1 = amber::linear_allocator::create(64, 128);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);
    auto exp_alloc = a1.allocate(30);
    REQUIRE(exp_alloc.has_value());
    std::uintptr_t p1 = reinterpret_cast<std::uintptr_t>(exp_alloc.value());
    REQUIRE((p1 % 64) == 0);
}

TEST_CASE("linear_allocator allocate(alignment, size)")
{
    void* p = nullptr;
    std::expected<void*, amber::alloc_error> exp_alloc;

    auto exp_a1 = amber::linear_allocator::create(128);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);

    exp_alloc = a1.allocate(1, 10);
    REQUIRE(exp_alloc.has_value());
    p = exp_alloc.value();
    REQUIRE(a1.buffer_offset() == 10);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 1) == 0);

    exp_alloc = a1.allocate(16, 20);
    REQUIRE(exp_alloc.has_value());
    p = exp_alloc.value();
    REQUIRE(a1.buffer_offset() == 36);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 16) == 0);

    exp_alloc = a1.allocate(4, 4);
    REQUIRE(exp_alloc.has_value());
    p = exp_alloc.value();
    REQUIRE(a1.buffer_offset() == 40);
    REQUIRE((reinterpret_cast<std::uintptr_t>(p) % 4) == 0);
}

TEST_CASE("linear_allocator allocate(size)")
{
    auto exp_a1 = amber::linear_allocator::create(128);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);

    auto exp_alloc1 = a1.allocate(13);
    REQUIRE(exp_alloc1.has_value());
    REQUIRE((reinterpret_cast<std::uintptr_t>(exp_alloc1.value()) % alignof(std::max_align_t)) == 0);
    REQUIRE(a1.buffer_offset() == 13);

    auto exp_alloc2 = a1.allocate(13);
    REQUIRE(exp_alloc2.has_value());
    REQUIRE((reinterpret_cast<std::uintptr_t>(exp_alloc2.value()) % alignof(std::max_align_t)) == 0);
    REQUIRE(a1.buffer_offset() == 29);

    auto exp_alloc3 = a1.allocate(37);
    REQUIRE(exp_alloc3.has_value());
    REQUIRE((reinterpret_cast<std::uintptr_t>(exp_alloc3.value()) % alignof(std::max_align_t)) == 0);
    REQUIRE(a1.buffer_offset() == 69);
}

TEST_CASE("linear_allocator allocate<T>(...)")
{
    int* int_ptr = nullptr;
    long* long_ptr = nullptr;
    auto exp_a1 = amber::linear_allocator::create(128);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);

    auto exp_alloc1 = a1.allocate<int>(10);
    REQUIRE(exp_alloc1.has_value());
    int_ptr = exp_alloc1.value();
    REQUIRE(int_ptr != nullptr);
    REQUIRE(a1.buffer_offset() == 4);
    REQUIRE((reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int)) == 0);

    auto exp_alloc2 = a1.allocate<long>(11);
    REQUIRE(exp_alloc2.has_value());
    long_ptr = exp_alloc2.value();
    REQUIRE(long_ptr != nullptr);
    REQUIRE(a1.buffer_offset() == 16);
    REQUIRE((reinterpret_cast<std::uintptr_t>(long_ptr) % alignof(long)) == 0);

    auto exp_alloc3 = a1.allocate<int>(12);
    REQUIRE(exp_alloc3.has_value());
    int_ptr = exp_alloc3.value();
    REQUIRE(int_ptr != nullptr);
    REQUIRE(a1.buffer_offset() == 20);
    REQUIRE((reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int)) == 0);
}

TEST_CASE("linear_allocator reset")
{
    auto exp_a1 = amber::linear_allocator::create(128);
    REQUIRE(exp_a1.has_value());
    amber::linear_allocator a1(std::move(exp_a1.value()));
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);

    a1.reset();
    REQUIRE(a1.buffer_offset() == 0);

    a1.allocate(24);
    REQUIRE(a1.buffer_offset() == 24);
    a1.reset();
    REQUIRE(a1.buffer_offset() == 0);
}

} // namespace amber_test
