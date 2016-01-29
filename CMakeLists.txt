cmake_minimum_required(VERSION 3.3)
project("OpenZE")
include(ExternalProject)

if(";${CMAKE_GENERATOR};" MATCHES ";Visual Studio;")
    set(BUILD_FLAGS "")
    set(DEBUG_BUILD_FLAGS "")
    set(RELEASE_BUILD_FLAGS "")
else()
    set(BUILD_FLAGS "-std=c++17 -Wextra -Wall -static")
    set(DEBUG_BUILD_FLAGS "-g")
    set(RELEASE_BUILD_FLAGS "-O3 -Os -s")
#else()
#    message(FATAL_ERROR "Ohoh, this is a problem. Please contact Meaty")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/debug)
    set(ADDITIONAL_BUILD_FLAGS ${DEBUG_BUILD_FLAGS})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/release)
    set(ADDITIONAL_BUILD_FLAGS ${RELEASE_BUILD_FLAGS})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BUILD_FLAGS} ${ADDITIONAL_BUILD_FLAGS}")

set(SRC
    src/main.cpp
)

add_executable(openze ${SRC})

set_target_properties(openze PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(openze)

