cmake_minimum_required(VERSION 3.3)
project(VDFS)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(vdfs STATIC ${SRC})


if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()


set_target_properties(vdfs PROPERTIES LINKER_LANGUAGE C)
