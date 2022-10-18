cmake_minimum_required(VERSION 3.16)

set(CORE_LIB my_project-core)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(PUBLIC_INCLUDES
    ${INC_DIR}/Application/Application.h
    ${INC_DIR}/Application/Args.h
    ${INC_DIR}/Config/IniConf.h
)

set(PRIVATE_INCLUDES
)

set(PRIVATE_SOURCES
    ${SRC_DIR}/Application/Application.cpp
    ${SRC_DIR}/Application/Args.cpp
    ${SRC_DIR}/Config/IniConf.cpp
)

set(SOURCES
    ${PUBLIC_INCLUDES}
    ${PRIVATE_INCLUDES}
    ${PRIVATE_SOURCES}
)

include_directories(
    ${SRC_DIR}
    ${INC_DIR}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(ThreadLib
    Threads::Threads
)

add_subdirectory(${LIB_DIR})

set(AllLibs
    ${ThreadLib}
)

add_library(${CORE_LIB}
    STATIC
    ${SOURCES}
)

target_link_libraries(${CORE_LIB}
    ${AllLibs}
)
