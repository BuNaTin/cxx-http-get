cmake_minimum_required(VERSION 3.16)

set(Project_name http_get-console)

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

target_link_libraries(${Project_name} http_get-core)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR BuNaTin)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "BuNaTin <k9687928424@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cpp simple single-thread sharing folder http server")
set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/cxx-http-get.cpack.cmake")
include(CPack)
