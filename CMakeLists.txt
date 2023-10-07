cmake_minimum_required(VERSION 3.18.0)

project(tape-sort LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build C++ Unit tests" ON)

add_subdirectory(src)
if (${BUILD_TESTS})
    add_subdirectory(tests)
endif ()