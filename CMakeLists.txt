cmake_minimum_required(VERSION 4.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
set(CMAKE_CXX_STANDARD 23)

set(AMBER_VERSION "0.0.1")

option(AMBER_TESTS "Build test executable")

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

add_library("${PROJECT_NAME}" ${AMBER_SOURCES})
target_include_directories("${PROJECT_NAME}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(AMBER_TESTS)
    message(STATUS "Building ${PROJECT_NAME} tests")
    enable_testing()
    add_subdirectory("test")
endif()

install(TARGETS "${PROJECT_NAME}")
