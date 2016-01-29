cmake_minimum_required(VERSION 3.3)
project(Renderer)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(renderer STATIC ${SRC})
set_target_properties(renderer PROPERTIES LINKER_LANGUAGE C)

