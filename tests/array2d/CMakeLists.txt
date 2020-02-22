cmake_minimum_required(VERSION 3.4)

project(array2d_test_console)

set (SOURCES
    console.cpp
    ../../core/utils/array2d.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ../../core/utils
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tests
)

add_test (NAME array2d-constructor  COMMAND ${PROJECT_NAME} array2d-constructor)
add_test (NAME array2d-resize       COMMAND ${PROJECT_NAME} array2d-resize)
add_test (NAME array2d-iterator     COMMAND ${PROJECT_NAME} array2d-iterator)
add_test (NAME array2d-indexes      COMMAND ${PROJECT_NAME} array2d-indexes)
