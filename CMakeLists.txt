cmake_minimum_required(VERSION 3.3)
project(OpenZE)
include(ExternalProject)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")

option(OPENZE_D3D11 "Enable D3D11-Build" OFF)
option(OPENZE_OPENAL "Enable OpenAL-Build" ON)

if(MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(ZE_BUILD_FLAGS "/D_CRT_SECURE_NO_WARNINGS /DNOMINMAX /MD /MP /EHsc /D_CRT_SECURE_NO_WARNINGS")
    set(ZE_DEBUG_BUILD_FLAGS "")
    set(ZE_RELEASE_BUILD_FLAGS "/arch:SSE2 /Ox /Ob2 /Oi /Ot /Oy /fp:fast /GF /FD /MT /GS-")
    #add_definitions("/D_CRT_SECURE_NO_WARNINGS /DNOMINMAX")
else()
    set(ZE_BUILD_FLAGS " -std=c++17 -Wextra -Wall ")
    set(ZE_DEBUG_BUILD_FLAGS "-g")
    set(ZE_RELEASE_BUILD_FLAGS "-O3 -s ")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ADDITIONAL_BUILD_FLAGS ${ZE_DEBUG_BUILD_FLAGS})
    add_definitions(-DZE_DEBUG)
else()
    set(ADDITIONAL_BUILD_FLAGS ${ZE_RELEASE_BUILD_FLAGS})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ZE_BUILD_FLAGS} ${ADDITIONAL_BUILD_FLAGS}")

include_directories(src)
include_directories(src/engine)
include_directories(lib/glm/glm)
include_directories(lib/bullet/src)
include_directories(lib/glfw/include)
include_directories(lib/rapi/renderer/include)
include_directories(lib/rapi/include)

if(OPENZE_OPENAL)
    include_directories(${OPENAL_INCLUDE_DIR})
endif()

set(BULLET_LIBRARIES Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamicsUtils BulletInverseDynamics LinearMath)
set(BULLET_MISC Bullet3Dynamics Bullet2FileLoader Bullet3Collision Bullet3Geometry Bullet3OpenCL_clew BulletFileLoader BulletWorldImporter BulletXmlWorldImporter ConvexDecomposition GIMPACTUtils HACD Test_BulletDynamics Test_BulletInverseDynamics Test_Collision)
set(LIBRARIES libdebug game_classes network physics storage utils glm zenconvert vdfs ${BULLET_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ZE_BUILD_FLAGS} ${ADDITIONAL_BUILD_FLAGS}")

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
add_subdirectory(src/zenconvert)
add_subdirectory(src/vdfs)

add_subdirectory(lib/glm)
add_subdirectory(lib/bullet)
add_subdirectory(lib/rapi)
add_subdirectory(lib/glfw)

# Folders for external libraries
foreach(infileName ${BULLET_LIBRARIES})
    set_target_properties (${infileName} PROPERTIES FOLDER lib)
endforeach(infileName)

foreach(infileName ${BULLET_MISC})
    set_target_properties (${infileName} PROPERTIES FOLDER lib)
endforeach(infileName)



if(OPENZE_D3D11)
    set_target_properties (RAPI_DX PROPERTIES FOLDER lib)
endif(OPENZE_D3D11)

set_target_properties (RAPI_GL PROPERTIES FOLDER lib)
set_target_properties (glfw PROPERTIES FOLDER lib)

# Move our libs to openZE
# Folders for game libraries
# TODO: Maybe move this to the cmakelists.txt of the actual library?
set_target_properties (libdebug PROPERTIES FOLDER openZE)
set_target_properties (game_classes PROPERTIES FOLDER openZE)
set_target_properties (gui PROPERTIES FOLDER openZE)
set_target_properties (input PROPERTIES FOLDER openZE)
set_target_properties (network PROPERTIES FOLDER openZE)
set_target_properties (physics PROPERTIES FOLDER openZE)
#set_target_properties (renderer_dx PROPERTIES FOLDER openZE)
set_target_properties (renderer_gl PROPERTIES FOLDER openZE)
set_target_properties (storage PROPERTIES FOLDER openZE)
set_target_properties (utils PROPERTIES FOLDER openZE)
set_target_properties (zenconvert PROPERTIES FOLDER openZE)
set_target_properties (vdfs PROPERTIES FOLDER openZE)

# source files
file(GLOB SRC
    src/engine/*.cpp
    src/engine/*.h
    src/engine/components/*.cpp
    src/engine/components/*.h
    )

file(GLOB SERVER_SRC
    src/engine/server/*.cpp
    src/engine/server/*.h
    )

file(GLOB GAME_SRC
    src/engine/game/*.cpp
    src/engine/game/*.h
    )

file(GLOB ZEN_CONVERT
    src/zenconvert/*.cpp
    src/zenconvert/*.h
    src/zenconvert/exporter/*.cpp
    src/zenconvert/exporter/*.h
    )

if(OPENZE_OPENAL)
    find_package(OpenAL REQUIRED)
    if(NOT OpenAL_FOUND)
        message(FATAL_ERROR "OpenAL not found")
    endif()
endif()

set(GAME_LIBRARIES sound input gui ${OPENAL_LIBRARY} ${GLFW_STATIC_LIBRARIES})

add_executable(convertzen ${ZEN_CONVERT})
target_link_libraries(convertzen utils vdfs)

add_executable(ozerver ${SRC})
set_target_properties(ozerver PROPERTIES LINKER_LANGUAGE C)
set_target_properties(ozerver PROPERTIES COMPILE_FLAGS -DZE_SERVER)
target_link_libraries(ozerver ${SERVER_LIBRARIES} ${LIBRARIES})
set_target_properties (ozerver PROPERTIES FOLDER openZE)

if(WIN32)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set( PSDK_PATH "C:/Program Files/Microsoft Platform SDK/Bin" )
    set( PSDK_INC "C:/Program Files/Microsoft Platform SDK/Include" )

    if( OPENZE_D3D11 )
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        find_path(DX_INCLUDE_PATH d3d11.h
            PATHS
            "$ENV{DXSDK_DIR}/Include"
            "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
            DOC "The directory where D3D11.h resides")

        find_library(D3D11_LIBRARY d3d11.lib
            PATHS
            "$ENV{DXSDK_DIR}/Lib/x64"
            "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x64"
            DOC "The directory where d3d11.lib resides")

        find_library(D3DX11_LIBRARY d3dx11.lib
            PATHS
            "$ENV{DXSDK_DIR}/Lib/x64"
            "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x64"
            DOC "The directory where d3dx11.lib resides")
    else()
        FIND_PATH(DX_INCLUDE_PATH d3d11.h
            PATHS
            "$ENV{DXSDK_DIR}/Include"
            "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
            DOC "The directory where D3D11.h resides")

        find_library(D3D11_LIBRARY d3d11.lib
            PATHS
            "$ENV{DXSDK_DIR}/Lib/x86"
            "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
            DOC "The directory where d3d11.lib resides")

        find_library(D3DX11_LIBRARY d3dx11.lib
            PATHS
            "$ENV{DXSDK_DIR}/Lib/x86"
            "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
            DOC "The directory where d3dx11.lib resides")
    else()
        set(DX_INCLUDE_PATH "")
        set(D3D11_LIBRARY "")
        set(D3DX11_LIBRARY "")
    endif()

    set( DX11_LIBRARIES ${D3D11_LIBRARY} ${D3DX11_LIBRARY} )

    include_directories(${DX_INCLUDE_PATH})

    add_executable(openze_dx ${SRC} ${GAME_SRC})
    set_target_properties(openze_dx PROPERTIES LINKER_LANGUAGE C)
    set_target_properties(openze_dx PROPERTIES COMPILE_FLAGS "-DZE_GAME -DRND_D3D11")
    target_link_libraries(openze_dx ${GAME_LIBRARIES} ${LIBRARIES} RAPI_DX glfw renderer_dx)
    set_target_properties (openze_dx PROPERTIES FOLDER openZE)
    else()

    endif()
endif()

add_executable(openze ${SRC} ${GAME_SRC})
set_target_properties(openze PROPERTIES LINKER_LANGUAGE C)
set_target_properties(openze PROPERTIES COMPILE_FLAGS "-DZE_GAME -DRND_GL")
target_link_libraries(openze ${GAME_LIBRARIES} ${LIBRARIES} RAPI_GL glfw renderer_gl vdfs zenconvert)

set_target_properties (openze PROPERTIES FOLDER openZE)
