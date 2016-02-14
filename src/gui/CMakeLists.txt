cmake_minimum_required(VERSION 3.3)
project(GUI)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(gui STATIC ${SRC})
set_target_properties(gui PROPERTIES LINKER_LANGUAGE C)
