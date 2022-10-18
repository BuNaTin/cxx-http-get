cmake_minimum_required(VERSION 3.16)

set(CORE_LIB http_get-core)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(PUBLIC_INCLUDES
    ${INC_DIR}/Application/Server.h
    ${INC_DIR}/Application/Args.h
)

set(PRIVATE_INCLUDES
    ${SRC_DIR}/utils/strFmt.h
    ${SRC_DIR}/utils/ClearFile.h
    ${SRC_DIR}/utils/FileSize.h
    ${SRC_DIR}/Http/Response.h
    ${SRC_DIR}/Http/Request.h
)

set(PRIVATE_SOURCES
    ${SRC_DIR}/Application/Server.cpp
    ${SRC_DIR}/Application/Args.cpp
    ${SRC_DIR}/utils/strFmt.cpp
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
