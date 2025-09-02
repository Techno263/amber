cmake_minimum_required(VERSION 4.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(AMBER_VERSION "0.0.1")

option(AMBER_TESTS "Build test executable" OFF)

string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" _ "${AMBER_VERSION}")
set(AMBER_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(AMBER_VERSION_MINOR "${CMAKE_MATCH_2}")
set(AMBER_VERSION_PATCH "${CMAKE_MATCH_3}")

project(amber
    VERSION "${AMBER_VERSION}"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/cmake/Util.cmake")

set(AMBER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
include("cmake/Sources.cmake")

find_package(mica REQUIRED)

add_library("${PROJECT_NAME}" ${AMBER_SOURCES} ${AMBER_HEADERS})
target_sources("${PROJECT_NAME}"
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "src"
        FILES ${AMBER_HEADERS}
)
target_compile_features("${PROJECT_NAME}"
    PUBLIC cxx_std_23
)
target_compile_options("${PROJECT_NAME}"
    PRIVATE -Wall -Wextra -Wpedantic -Werror
)
target_include_directories("${PROJECT_NAME}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries("${PROJECT_NAME}"
    PUBLIC
        mica::mica
)

if(AMBER_TESTS)
    message(STATUS "Building ${PROJECT_NAME} tests")
    enable_testing()
    add_subdirectory("test")
endif()

install(
    TARGETS "${PROJECT_NAME}"
    FILE_SET HEADERS
)
