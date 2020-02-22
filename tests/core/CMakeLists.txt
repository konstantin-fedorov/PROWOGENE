cmake_minimum_required(VERSION 3.1)

project(core_test_console)

set (SOURCES
    ../../console/console.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC
    prowogene_core
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tests
)

add_test (NAME core-physical-map COMMAND ${PROJECT_NAME} ../../../config/physical_map.json)
add_test (NAME core-height-map   COMMAND ${PROJECT_NAME} ../../../config/height_map.json)
add_test (NAME core-3d-map       COMMAND ${PROJECT_NAME} ../../../config/3d_map.json)