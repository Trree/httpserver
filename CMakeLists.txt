cmake_minimum_required(VERSION 3.3)
project (httpserver)
SET(CMAKE_C_COMPILER "g++") 
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#"${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#"${PROJECT_BINARY_DIR}/share/tcp_server.h"
#
#)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# package information
set(PACKAGE_NAME      "httpserver")
set(PACKAGE_VERSION   "0.0.1-dev")
set(PACKAGE_STRING    "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/Trree/httpserver/issues")

option(HTTPSERVER_ENABLE_TESTING "Enable testing of the library." ON)

set(TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(HTTP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/httpserver")
set(
  HTTP_DIR_PREFIXES
  "${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_CURRENT_BINARY_DIR}"
)

add_library(http_runtime SHARED
    ${HTTP_DIR}/http/response.cpp 
)

target_sources(http_runtime PRIVATE
    ${HTTP_DIR}/handle_string.cpp
    ${HTTP_DIR}/inetaddress.cpp
    ${HTTP_DIR}/connection.cpp
    ${HTTP_DIR}/server.cpp
    ${HTTP_DIR}/main.cpp
)

target_include_directories(http_runtime
  PUBLIC
    ${HTTP_INCLUDE_DIRECTORIES}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
ADD_DEFINITIONS(-std=c++11 -g3 -W -Wall -pedantic -D_GNU_SOURCE)
include(${CMAKE_MODULE_PATH}/clang-cxx-dev-tools.cmake)
#include(${CMAKE_MODULE_PATH}/common.cmake)

# add the executable
add_executable(${PROJECT_NAME}  
  ${HTTP_DIR}/main.cpp
  #$<TARGET_OBJECTS:http_runtime>
)

target_link_libraries(${PROJECT_NAME} http_runtime)

if (HTTPSERVER_ENABLE_TESTING)
  enable_testing()
  #add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
  add_subdirectory(test)
endif()

