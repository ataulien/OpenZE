cmake_minimum_required(VERSION 3.3)
project(Renderer)

file(GLOB SRC
    *.cpp
    *.h
	visuals/*.cpp
	visuals/*.h
)

add_library(renderer_gl STATIC ${SRC})
set_target_properties(renderer_gl PROPERTIES LINKER_LANGUAGE C)
set_target_properties(renderer_gl PROPERTIES COMPILE_FLAGS "-DRND_GL -DZE_GAME")

if(OPENZE_D3D11)
	if( WIN32 )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	
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
	else()
		SET(DX_INCLUDE_PATH "")
		SET(D3D11_LIBRARY "")
		SET(D3DX11_LIBRARY "")

	endif()


	add_library(renderer_dx STATIC ${SRC})
	set_target_properties(renderer_dx PROPERTIES LINKER_LANGUAGE C)
	set_target_properties(renderer_dx PROPERTIES COMPILE_FLAGS "-DRND_D3D11 -DZE_GAME")
endif()
