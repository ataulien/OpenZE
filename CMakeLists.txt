cmake_minimum_required(VERSION 3.3)
project(OpenZE)
include(ExternalProject)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")

if(";${CMAKE_GENERATOR};" MATCHES ";Visual Studio;")
    set(ZE_BUILD_FLAGS "")
    set(ZE_DEBUG_BUILD_FLAGS "")
    set(ZE_RELEASE_BUILD_FLAGS "")
else()
    set(ZE_BUILD_FLAGS " -std=c++17 -Wextra -Wall ")
    set(ZE_DEBUG_BUILD_FLAGS "-g")
    set(ZE_RELEASE_BUILD_FLAGS "-O3 -Os -s")
endif()

include_directories(src)
include_directories(src/engine)
include_directories(lib/glm/glm)
include_directories(lib/bullet/src)
include_directories(${OPENAL_INCLUDE_DIR})

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
add_subdirectory(lib/rapi)

set(BULLET_LIBRARIES Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamicsUtils BulletInverseDynamics LinearMath)
set(LIBRARIES debug game_classes network physics storage utils glm ${BULLET_LIBRARIES})

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


find_package(SFML 2 REQUIRED system window graphics)

if(!SFML_FOUND)
    message(FATAL_ERROR "SFML not found")
endif()

include_directories(${SFML_INCLUDE_DIR})

find_package(OpenAL REQUIRED)

if(!OPENAL_FOUND)
    message(FATAL_ERROR "OpenAL not found")
endif()

set(GAME_LIBRARIES renderer sound input gui ${OPENAL_LIBRARY} ${SFML_LIBRARIES})

add_executable(openze ${SRC})
set_target_properties(openze PROPERTIES LINKER_LANGUAGE C)
set_target_properties(openze PROPERTIES COMPILE_FLAGS -DZE_GAME)
target_link_libraries(openze ${GAME_LIBRARIES} ${LIBRARIES})

add_executable(ozerver ${SRC})
set_target_properties(ozerver PROPERTIES LINKER_LANGUAGE C)
set_target_properties(ozerver PROPERTIES COMPILE_FLAGS -DZE_SERVER)
target_link_libraries(ozerver ${SERVER_LIBRARIES} ${LIBRARIES})
