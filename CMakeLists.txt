cmake_minimum_required(VERSION 3.4)

project(PROWOGENE)

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

option (PROWOGENE_ENABLE_TESTS  "Build tests" OFF)

add_subdirectory(console)
add_subdirectory(core)

if (PROWOGENE_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests/array2d)
    add_subdirectory(tests/core)
    add_subdirectory(tests/json)
endif()