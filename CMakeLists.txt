cmake_minimum_required(VERSION 3.3)
project(OpenZE)
include(ExternalProject)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")
set(BUILD_CPU_DEMOS OFF CACHE BOOL "")
set(BUILD_EXAMPLES OFF CACHE BOOL "")
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
set(BUILD_TESTS OFF CACHE BOOL "")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "")
set(CONNECTOR_FCGI OFF CACHE BOOL "")
set(CONNECTOR_HTTP OFF CACHE BOOL "")
set(ENABLE_FIREBIRD OFF CACHE BOOL "")
set(ENABLE_HARU OFF CACHE BOOL "")
set(ENABLE_LIBWTTEST OFF CACHE BOOL "")
set(ENABLE_PANGO OFF CACHE BOOL "")
set(ENABLE_QT4 OFF CACHE BOOL "")
set(ENABLE_OPENGL OFF CACHE BOOL "")

option(SERVER_USE_MYSQL "Use MySQL" ON)
if(SERVER_USE_MYSQL)
    #set(SERVER_LIBRARIES wtdbomysql)
else()
    #set(SERVER_LIBRARIES wtdbosqlite3)
endif()

if(";${CMAKE_GENERATOR};" MATCHES ";Visual Studio;")
    set(ZE_BUILD_FLAGS "")
    set(ZE_DEBUG_BUILD_FLAGS "")
    set(ZE_RELEASE_BUILD_FLAGS "")
else()
    set(ZE_BUILD_FLAGS " -std=c++17 -Wextra -Wall ")
    set(ZE_DEBUG_BUILD_FLAGS "-g")
    set(ZE_RELEASE_BUILD_FLAGS "-O3 -Os -s")
endif()

find_package(OpenAL REQUIRED)

if(!OPENAL_FOUND)
    message(FATAL_ERROR "OpenAL not found")
endif()

include_directories(src)
include_directories(src/engine)
include_directories(lib/glm/glm)
include_directories(lib/bullet/src)
include_directories(lib/wt/src/Wt/Dbo)
include_directories(${OPENAL_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ADDITIONAL_BUILD_FLAGS ${ZE_DEBUG_BUILD_FLAGS})
    add_definitions(-DZE_DEBUG)
else()
    set(ADDITIONAL_BUILD_FLAGS ${ZE_RELEASE_BUILD_FLAGS})
endif()

add_subdirectory(src/debug)
add_subdirectory(src/game_classes)
add_subdirectory(src/gui)
add_subdirectory(src/input)
add_subdirectory(src/network)
add_subdirectory(src/physics)
add_subdirectory(src/renderer)
add_subdirectory(src/sound)
add_subdirectory(src/storage)
add_subdirectory(src/utils)

add_subdirectory(lib/glm)
add_subdirectory(lib/bullet)
#add_subdirectory(lib/wt)#TODO: Don't compile the whole project, just the Dbo-ORM part.

set(BULLET_LIBRARIES Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamicsUtils BulletInverseDynamics LinearMath )
set(GAME_LIBRARIES renderer sound input gui)#wtdbosqlite3
set(LIBRARIES debug game_classes network physics storage utils glm wtdbo ${OPENAL_LIBRARY} ${BULLET_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ZE_BUILD_FLAGS} ${ADDITIONAL_BUILD_FLAGS}")

file(GLOB SRC
    src/engine/*.cpp
    src/engine/*.h
)

file(GLOB SERVER_SRC
    src/engine/server/*.cpp
    src/engine/server/*.h
)

file(GLOB GAME_SRC
    src/engine/game/*.cpp
    src/engine/game/*.h
)

add_executable(openze ${SRC})
set_target_properties(openze PROPERTIES LINKER_LANGUAGE C)
set_target_properties(openze PROPERTIES COMPILE_FLAGS -DZE_GAME)
target_link_libraries(openze ${GAME_LIBRARIES} ${LIBRARIES})

add_executable(ozerver ${SRC})
set_target_properties(ozerver PROPERTIES LINKER_LANGUAGE C)
set_target_properties(ozerver PROPERTIES COMPILE_FLAGS -DZE_SERVER)
target_link_libraries(ozerver ${SERVER_LIBRARIES} ${LIBRARIES})
