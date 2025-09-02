set(AMBER_HEADERS
    aligned_buffer.hpp
    amber.hpp
    bitwise_enum.hpp
    concept.hpp
    linear_allocator.hpp
    linear_allocator.inl
    malloc_buffer.hpp
    mmap_buffer.hpp
    pool_allocator.hpp
    pool_allocator.inl
    stack_allocator.hpp
    stack_allocator.inl
    util.hpp
    util.inl
)

set(AMBER_SOURCES
    aligned_buffer.cpp
    linear_allocator.cpp
    malloc_buffer.cpp
    mmap_buffer.cpp
    pool_allocator.cpp
    stack_allocator.cpp
    util.cpp
)

prepend_paths(
    "${AMBER_HEADERS}"
    "src/amber"
    "AMBER_HEADERS"
)

prepend_paths(
    "${AMBER_SOURCES}"
    "src/amber"
    "AMBER_SOURCES"
)
