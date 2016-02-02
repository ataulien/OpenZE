cmake_minimum_required(VERSION 3.3)
project(LibDebug)

if(";${CMAKE_GENERATOR};" MATCHES ";Visual Studio;")
    set(ZE_BUILD_FLAGS "")
    set(ZE_DEBUG_BUILD_FLAGS "")
    set(ZE_RELEASE_BUILD_FLAGS "")
else()
    set(ZE_BUILD_FLAGS " -std=c++17 -Wextra -Wall ")
    set(ZE_DEBUG_BUILD_FLAGS "-g")
    set(ZE_RELEASE_BUILD_FLAGS "-O3 -Os -s")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ADDITIONAL_BUILD_FLAGS ${ZE_DEBUG_BUILD_FLAGS})
    add_definitions(-DZE_DEBUG)
else()
    set(ADDITIONAL_BUILD_FLAGS ${ZE_RELEASE_BUILD_FLAGS})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ZE_BUILD_FLAGS} ${ADDITIONAL_BUILD_FLAGS}")

file(GLOB SRC
    *.cpp
    *.h
)

add_library(libdebug STATIC ${SRC})
set_target_properties(libdebug PROPERTIES LINKER_LANGUAGE C)
