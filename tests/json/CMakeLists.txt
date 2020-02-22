cmake_minimum_required(VERSION 3.4)

project(json_test_console)

set (SOURCES
    json_test_console.cpp
    ${CMAKE_SOURCE_DIR}/core/utils/json.cpp
    ${CMAKE_SOURCE_DIR}/core/utils/json.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/core
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tests
)

add_test (NAME json-pos-int           COMMAND ${PROJECT_NAME} json-pos-int)
add_test (NAME json-neg-int           COMMAND ${PROJECT_NAME} json-neg-int)
add_test (NAME json-pos-float         COMMAND ${PROJECT_NAME} json-pos-float)
add_test (NAME json-neg-float         COMMAND ${PROJECT_NAME} json-neg-float)
add_test (NAME json-pos-float-pos-exp COMMAND ${PROJECT_NAME} json-pos-float-pos-exp)
add_test (NAME json-pos-float-neg-exp COMMAND ${PROJECT_NAME} json-pos-float-neg-exp)
add_test (NAME json-neg-float-pos-exp COMMAND ${PROJECT_NAME} json-neg-float-pos-exp)
add_test (NAME json-neg-float-neg-exp COMMAND ${PROJECT_NAME} json-neg-float-neg-exp)
add_test (NAME json-bool-true         COMMAND ${PROJECT_NAME} json-bool-true)
add_test (NAME json-bool-false        COMMAND ${PROJECT_NAME} json-bool-false)
add_test (NAME json-null              COMMAND ${PROJECT_NAME} json-null)
add_test (NAME json-string            COMMAND ${PROJECT_NAME} json-string)
add_test (NAME json-array             COMMAND ${PROJECT_NAME} json-array)
add_test (NAME json-object            COMMAND ${PROJECT_NAME} json-object)
add_test (NAME json-file              COMMAND ${PROJECT_NAME} json-file)
