set(AMBER_UNITTEST_SOURCES
    linear_allocator_test.cpp
    util_test.cpp
)

prepend_paths(
    "${AMBER_UNITTEST_SOURCES}"
    "src/amber_test"
    "AMBER_UNITTEST_SOURCES"
)
