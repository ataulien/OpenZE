cmake_minimum_required(VERSION 3.3)
project(Renderer)

file(GLOB SRC
    *.cpp
    *.h
)


IF( WIN32 )
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DEBUG")
	SET( PSDK_PATH "C:/Program Files/Microsoft Platform SDK/Bin" )
	SET( PSDK_INC "C:/Program Files/Microsoft Platform SDK/Include" )

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


if(WIN32)
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		link_directories("lib/glew/x64")
		link_directories("lib/glfw/x64")
	else()
		link_directories("lib/glew/Win32")
		link_directories("lib/glfw/Win32")
	endif()

	add_executable(renderer_test_DX WIN32 ${SRC})
	add_executable(renderer_test_GL WIN32 ${SRC})
	
	target_link_libraries(renderer_test_DX RAPI_DX)
	
	target_compile_options(renderer_test_DX PUBLIC -DRND_D3D11)
else()
endif()

add_library(renderer STATIC ${SRC})
set_target_properties(renderer PROPERTIES LINKER_LANGUAGE C)

