find_package(Catch2 REQUIRED COMPONENTS Catch2 Catch2Main)

set(UNITTEST_NAME "${PROJECT_NAME}_unittest")

set(AMBER_UNITTEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/Sources.cmake")

add_executable("${UNITTEST_NAME}" ${AMBER_UNITTEST_SOURCES})
target_link_libraries("${UNITTEST_NAME}"
    PRIVATE
        "${PROJECT_NAME}"
        Catch2::Catch2
        Catch2::Catch2Main
)

add_test(
    NAME "${UNITTEST_NAME}"
    COMMAND "${UNITTEST_NAME}"
)
