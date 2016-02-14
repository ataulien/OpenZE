cmake_minimum_required(VERSION 3.3)
project(Utils)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(utils STATIC ${SRC})
set_target_properties(utils PROPERTIES LINKER_LANGUAGE C)
