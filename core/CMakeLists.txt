cmake_minimum_required(VERSION 3.16)

set(CORE_LIB http_get-core)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set(PUBLIC_INCLUDES
    ${INC_DIR}/Application/Application.h
    ${INC_DIR}/Application/Args.h
)

set(PRIVATE_INCLUDES
    ${SRC_DIR}/utils/strFmt.h
    ${SRC_DIR}/utils/ClearFile.h
    ${SRC_DIR}/utils/FileSize.h
    ${SRC_DIR}/Http/Response.h
    ${SRC_DIR}/Http/Request.h
    ${SRC_DIR}/Http/Server.h
    ${SRC_DIR}/Http/initHandlers.h
)

set(PRIVATE_SOURCES
    ${SRC_DIR}/Application/Application.cpp
    ${SRC_DIR}/Application/Args.cpp
    ${SRC_DIR}/Http/Server.cpp
    ${SRC_DIR}/Http/initHandlers.cpp
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

install(TARGETS ${Project_name} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR BuNaTin)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "BuNaTin <k9687928424@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cpp simple single-thread sharing folder http server")
set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/lib-cxx-http-get.cpack.cmake")

set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 2)
set(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

include(CPack)
