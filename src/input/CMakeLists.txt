cmake_minimum_required(VERSION 3.3)
project(Input)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(input STATIC ${SRC})
set_target_properties(input PROPERTIES LINKER_LANGUAGE C)
