cmake_minimum_required(VERSION 3.1)

project(prowogene_console)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC
    prowogene_core
)