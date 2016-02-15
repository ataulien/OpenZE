cmake_minimum_required(VERSION 3.3)
project(OpenZE)
include(ExternalProject)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")

if(MSVC)
    set(ZE_BUILD_FLAGS "/D_CRT_SECURE_NO_WARNINGS /DNOMINMAX")
    set(ZE_DEBUG_BUILD_FLAGS "")
    set(ZE_RELEASE_BUILD_FLAGS "/arch:SSE2 /Ox /Ob2 /Oi /Ot /Oy /fp:fast /GF /FD /MT /GS-")
	#add_definitions("/D_CRT_SECURE_NO_WARNINGS /DNOMINMAX")
else()
    set(ZE_BUILD_FLAGS " -std=c++17 -Wextra -Wall -H ")
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

include_directories(${OPENAL_INCLUDE_DIR})

set(BULLET_LIBRARIES Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamicsUtils BulletInverseDynamics LinearMath)
set(LIBRARIES debug game_classes network physics storage utils glm zenconvert ${BULLET_LIBRARIES})

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

add_subdirectory(lib/glm)
add_subdirectory(lib/bullet)
add_subdirectory(lib/rapi)
add_subdirectory(lib/glfw)


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
find_package(OpenAL REQUIRED)

if(!OPENAL_FOUND)
    message(FATAL_ERROR "OpenAL not found")
endif()

set(GAME_LIBRARIES renderer sound input gui ${OPENAL_LIBRARY} ${GLFW_STATIC_LIBRARIES})

add_executable(convertzen ${ZEN_CONVERT})
target_link_libraries(convertzen utils)

add_executable(render_test_GL src/renderer/renderer_test.cpp)
set_target_properties(render_test_GL PROPERTIES LINKER_LANGUAGE C)
set_target_properties(render_test_GL PROPERTIES COMPILE_FLAGS -DZE_SERVER)
target_link_libraries(render_test_GL ${LIBRARIES} ${GAME_LIBRARIES})

add_executable(ozerver ${SRC})
set_target_properties(ozerver PROPERTIES LINKER_LANGUAGE C)
set_target_properties(ozerver PROPERTIES COMPILE_FLAGS -DZE_SERVER)
target_link_libraries(ozerver ${SERVER_LIBRARIES} ${LIBRARIES})

if(WIN32)

	IF( WIN32 )
	
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		SET( PSDK_PATH "C:/Program Files/Microsoft Platform SDK/Bin" )
		SET( PSDK_INC "C:/Program Files/Microsoft Platform SDK/Include" )

	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		link_directories("lib/glew/x64")
		link_directories("lib/glfw/x64")
	else()
		link_directories("lib/glew/Win32")
		link_directories("lib/glfw/Win32")
	endif()
		
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		FIND_PATH(DX_INCLUDE_PATH d3d11.h
			PATHS
				"$ENV{DXSDK_DIR}/Include"
				"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
			DOC "The directory where D3D11.h resides")


		FIND_LIBRARY(D3D11_LIBRARY d3d11.lib
			PATHS
				"$ENV{DXSDK_DIR}/Lib/x64"
				"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x64"
			DOC "The directory where d3d11.lib resides")

		FIND_LIBRARY(D3DX11_LIBRARY d3dx11.lib
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


		FIND_LIBRARY(D3D11_LIBRARY d3d11.lib
			PATHS
				"$ENV{DXSDK_DIR}/Lib/x86"
				"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
			DOC "The directory where d3d11.lib resides")

		FIND_LIBRARY(D3DX11_LIBRARY d3dx11.lib
			PATHS
				"$ENV{DXSDK_DIR}/Lib/x86"
				"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
			DOC "The directory where d3dx11.lib resides")
	endif()
			
		SET( DX11_LIBRARIES ${D3D11_LIBRARY} ${D3DX11_LIBRARY} )
		
		include_directories(${DX_INCLUDE_PATH})
	ELSE()
		
	ENDIF() 

	add_executable(openze_dx ${SRC} ${GAME_SRC})
	set_target_properties(openze_dx PROPERTIES LINKER_LANGUAGE C)
	set_target_properties(openze_dx PROPERTIES COMPILE_FLAGS "-DZE_GAME -DRND_D3D11")
	target_link_libraries(openze_dx ${GAME_LIBRARIES} ${LIBRARIES} RAPI_DX glfw)
endif()

add_executable(openze ${SRC} ${GAME_SRC})
set_target_properties(openze PROPERTIES LINKER_LANGUAGE C)
set_target_properties(openze PROPERTIES COMPILE_FLAGS "-DZE_GAME -DRND_GL")
target_link_libraries(openze ${GAME_LIBRARIES} ${LIBRARIES} RAPI_GL glfw)





