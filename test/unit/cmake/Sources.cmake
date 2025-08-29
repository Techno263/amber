set(AMBER_UNITTEST_SOURCES
    aligned_buffer_test.cpp
    linear_allocator_test.cpp
    malloc_buffer_test.cpp
    mmap_buffer_test.cpp
    pool_allocator_test.cpp
    stack_allocator_test.cpp
    util_test.cpp
)

prepend_paths(
    "${AMBER_UNITTEST_SOURCES}"
    "src/amber_test"
    "AMBER_UNITTEST_SOURCES"
)
