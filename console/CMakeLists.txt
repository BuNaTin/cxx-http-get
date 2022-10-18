cmake_minimum_required(VERSION 3.16)

set(Project_name my_project-console)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
    ${SRC_DIR}
)

set(SOURCES
    ${SRC_DIR}/Arguments.h
)

add_executable(${Project_name}
    main.cpp
    ${SOURCES}
)

target_link_libraries(${Project_name} my_project-core)
