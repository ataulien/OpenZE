cmake_minimum_required(VERSION 3.3)
project(Sound)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(sound STATIC ${SRC})
set_target_properties(sound PROPERTIES LINKER_LANGUAGE C)
