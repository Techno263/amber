#include <amber/malloc_buffer.hpp>
#include <amber/pool_allocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <utility>

namespace amber_test {

TEST_CASE("pool_allocator move constructor/assignment")
{
    auto&& exp_buffer = amber::malloc_buffer::create(128);
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer = std::move(exp_buffer).value();

    auto&& exp_alloc = amber::pool_allocator::create(buffer, 8);
    REQUIRE(exp_alloc.has_value());
    amber::pool_allocator a1(std::move(exp_alloc).value());
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.entry_size() == 8);
    REQUIRE(a1.entry_count() == 16);
    REQUIRE(a1.entry_allocate_count() == 0);

    amber::pool_allocator a2(std::move(a1));
    REQUIRE(a1.buffer_size() == 0);
    REQUIRE(a1.entry_size() == 0);
    REQUIRE(a1.entry_count() == 0);
    REQUIRE(a1.entry_allocate_count() == 0);
    REQUIRE(a2.buffer_size() == 128);
    REQUIRE(a2.entry_size() == 8);
    REQUIRE(a2.entry_count() == 16);
    REQUIRE(a2.entry_allocate_count() == 0);

    a1 = std::move(a2);
    REQUIRE(a1.buffer_size() == 128);
    REQUIRE(a1.entry_size() == 8);
    REQUIRE(a1.entry_count() == 16);
    REQUIRE(a1.entry_allocate_count() == 0);
    REQUIRE(a2.buffer_size() == 0);
    REQUIRE(a2.entry_size() == 0);
    REQUIRE(a2.entry_count() == 0);
    REQUIRE(a2.entry_allocate_count() == 0);
}

TEST_CASE("pool_allocator allocate()/free(ptr)")
{
    auto&& exp_buffer = amber::malloc_buffer::create(32);
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer = std::move(exp_buffer).value();

    auto&& exp_alloc = amber::pool_allocator::create(buffer, 8);
    REQUIRE(exp_alloc.has_value());
    amber::pool_allocator allocator(std::move(exp_alloc).value());
    REQUIRE(allocator.buffer_size() == 32);
    REQUIRE(allocator.entry_size() == 8);
    REQUIRE(allocator.entry_count() == 4);
    REQUIRE(allocator.entry_allocate_count() == 0);

    auto exp_a1 = allocator.allocate();
    REQUIRE(exp_a1.has_value());
    REQUIRE(allocator.entry_allocate_count() == 1);
    REQUIRE(allocator.entry_free_count() == 3);

    auto exp_a2 = allocator.allocate();
    REQUIRE(exp_a2.has_value());
    REQUIRE(allocator.entry_allocate_count() == 2);
    REQUIRE(allocator.entry_free_count() == 2);

    auto exp_a3 = allocator.allocate();
    REQUIRE(exp_a3.has_value());
    REQUIRE(allocator.entry_allocate_count() == 3);
    REQUIRE(allocator.entry_free_count() == 1);

    auto exp_a4 = allocator.allocate();
    REQUIRE(exp_a4.has_value());
    REQUIRE(allocator.entry_allocate_count() == 4);
    REQUIRE(allocator.entry_free_count() == 0);

    auto exp_a5 = allocator.allocate();
    REQUIRE_FALSE(exp_a5.has_value());
    REQUIRE(exp_a5.error() == "out of capacity");

    allocator.free(exp_a1.value());
    REQUIRE(allocator.entry_allocate_count() == 3);
    REQUIRE(allocator.entry_free_count() == 1);

    allocator.free(exp_a2.value());
    REQUIRE(allocator.entry_allocate_count() == 2);
    REQUIRE(allocator.entry_free_count() == 2);

    allocator.free(exp_a3.value());
    REQUIRE(allocator.entry_allocate_count() == 1);
    REQUIRE(allocator.entry_free_count() == 3);

    allocator.free(exp_a4.value());
    REQUIRE(allocator.entry_allocate_count() == 0);
    REQUIRE(allocator.entry_free_count() == 4);
}

TEST_CASE("pool_allocator allocate<T>()/free<T>(ptr)")
{
    struct foo {
    public:
        char a[6];
    };
    static_assert(sizeof(foo) == 6);

    auto&& exp_buffer = amber::malloc_buffer::create(32);
    REQUIRE(exp_buffer.has_value());
    amber::malloc_buffer buffer = std::move(exp_buffer).value();

    auto&& exp_alloc = amber::pool_allocator::create(buffer, 8);
    REQUIRE(exp_alloc.has_value());
    amber::pool_allocator allocator(std::move(exp_alloc).value());
    REQUIRE(allocator.buffer_size() == 32);
    REQUIRE(allocator.entry_size() == 8);
    REQUIRE(allocator.entry_count() == 4);
    REQUIRE(allocator.entry_allocate_count() == 0);

    auto exp_a1 = allocator.allocate<foo>();
    REQUIRE(exp_a1.has_value());
    REQUIRE(allocator.entry_allocate_count() == 1);
    REQUIRE(allocator.entry_free_count() == 3);

    auto exp_a2 = allocator.allocate<foo>();
    REQUIRE(exp_a2.has_value());
    REQUIRE(allocator.entry_allocate_count() == 2);
    REQUIRE(allocator.entry_free_count() == 2);

    auto exp_a3 = allocator.allocate<foo>();
    REQUIRE(exp_a3.has_value());
    REQUIRE(allocator.entry_allocate_count() == 3);
    REQUIRE(allocator.entry_free_count() == 1);

    auto exp_a4 = allocator.allocate<foo>();
    REQUIRE(exp_a4.has_value());
    REQUIRE(allocator.entry_allocate_count() == 4);
    REQUIRE(allocator.entry_free_count() == 0);

    auto exp_a5 = allocator.allocate<foo>();
    REQUIRE_FALSE(exp_a5.has_value());
    REQUIRE(exp_a5.error() == "out of capacity");

    allocator.free<foo>(exp_a1.value());
    REQUIRE(allocator.entry_allocate_count() == 3);
    REQUIRE(allocator.entry_free_count() == 1);

    allocator.free<foo>(exp_a2.value());
    REQUIRE(allocator.entry_allocate_count() == 2);
    REQUIRE(allocator.entry_free_count() == 2);

    allocator.free<foo>(exp_a3.value());
    REQUIRE(allocator.entry_allocate_count() == 1);
    REQUIRE(allocator.entry_free_count() == 3);

    allocator.free<foo>(exp_a4.value());
    REQUIRE(allocator.entry_allocate_count() == 0);
    REQUIRE(allocator.entry_free_count() == 4);

    struct foo2 {
    public:
        int a;
        int b;
        int c;
    };
    static_assert(sizeof(foo2) == 12);

    auto exp_a6 = allocator.allocate<foo2>();
    REQUIRE_FALSE(exp_a6.has_value());
    REQUIRE(exp_a6.error() == "type size too large");
}

} // namespace amber_test
