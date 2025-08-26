#include <amber/malloc_buffer.hpp>
#include <amber/stack_allocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <utility>

namespace amber_test {

namespace {

template<std::size_t N>
struct foo {
    char a[N];
};

} // unnamed namespace

TEST_CASE("stack_allocator move constructor/assignment")
{
    auto exp_buffer = amber::malloc_buffer::create(128);
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer = std::move(exp_buffer).value();
    REQUIRE(buffer.size() == 128);

    auto exp_alloc = amber::stack_allocator::create(buffer);
    REQUIRE(exp_alloc.has_value());
    amber::stack_allocator a1(std::move(exp_alloc).value());
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);

    auto exp_tmp1 = a1.allocate(8);
    REQUIRE(exp_tmp1.has_value());

    amber::stack_allocator a2(std::move(a1));
    REQUIRE(a1.buffer_size() == 0);
    REQUIRE(a1.buffer_offset() == 0);
    REQUIRE(a2.buffer_size() == 128);
    REQUIRE(a2.buffer_offset() == 24);

    a2.free(exp_tmp1.value());

    a1 = std::move(a2);
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.buffer_offset() == 0);
    REQUIRE(a2.buffer_size() == 0);
    REQUIRE(a2.buffer_offset() == 0);
}

TEST_CASE("stack_allocator allocate(...)/free(ptr)")
{
    auto exp_buffer = amber::malloc_buffer::create(128);
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer = std::move(exp_buffer).value();

    std::uintptr_t buffer_addr = reinterpret_cast<std::uintptr_t>(buffer.buffer().data());

    auto exp_alloc = amber::stack_allocator::create(buffer);
    REQUIRE(exp_alloc.has_value());
    amber::stack_allocator allocator(std::move(exp_alloc).value());
    REQUIRE(allocator.buffer_size() == 128);
    REQUIRE(allocator.buffer_offset() == 0);

    auto exp_a1 = allocator.allocate(30);
    REQUIRE(exp_a1.has_value());
    // TODO: hard-coding 16 here is not crossplatform, should find a better solution
    REQUIRE(reinterpret_cast<std::uintptr_t>(exp_a1.value()) - buffer_addr == 16);
    REQUIRE(allocator.buffer_offset() == 46);

    auto exp_a2 = allocator.allocate(38);
    REQUIRE(exp_a2.has_value());
    REQUIRE(reinterpret_cast<std::uintptr_t>(exp_a2.value()) - buffer_addr == 64);
    REQUIRE(allocator.buffer_offset() == 102);

    auto exp_a3 = allocator.allocate(60);
    REQUIRE_FALSE(exp_a3.has_value());
    REQUIRE(exp_a3.error() == "out of capacity");

    allocator.free(exp_a2.value());
    REQUIRE(allocator.buffer_offset() == 46);

    allocator.free(exp_a1.value());
    REQUIRE(allocator.buffer_offset() == 0);
}

TEST_CASE("stack_allocator allocate<T>()/free<T>(ptr)")
{
    auto exp_buffer = amber::malloc_buffer::create(128);
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer = std::move(exp_buffer).value();

    std::uintptr_t buffer_addr = reinterpret_cast<std::uintptr_t>(buffer.buffer().data());

    auto exp_alloc = amber::stack_allocator::create(buffer);
    REQUIRE(exp_alloc.has_value());
    amber::stack_allocator allocator(std::move(exp_alloc).value());
    REQUIRE(allocator.buffer_size() == 128);
    REQUIRE(allocator.buffer_offset() == 0);

    auto exp_a1 = allocator.allocate<foo<30>>();
    REQUIRE(exp_a1.has_value());
    // 8 comes from alloc_header alignment, this hard-coded value is not crossplatform
    REQUIRE(reinterpret_cast<std::uintptr_t>(exp_a1.value()) - buffer_addr == 8);
    REQUIRE(allocator.buffer_offset() == 38);

    auto exp_a2 = allocator.allocate<foo<38>>();
    REQUIRE(exp_a2.has_value());
    REQUIRE(reinterpret_cast<std::uintptr_t>(exp_a2.value()) - buffer_addr == 48);
    REQUIRE(allocator.buffer_offset() == 86);

    auto exp_a3 = allocator.allocate<foo<60>>();
    REQUIRE_FALSE(exp_a3.has_value());
    REQUIRE(exp_a3.error() == "out of capacity");

    allocator.free(exp_a2.value());
    REQUIRE(allocator.buffer_offset() == 38);

    allocator.free(exp_a1.value());
    REQUIRE(allocator.buffer_offset() == 0);

    auto exp_a4 = allocator.allocate<foo<256>>();
    REQUIRE_FALSE(exp_a4.has_value());
    REQUIRE(exp_a4.error() == "out of capacity");
}

} // namespace amber_test
