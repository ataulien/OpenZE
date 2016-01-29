cmake_minimum_required(VERSION 3.3)
project(Network)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(network STATIC ${SRC})
set_target_properties(network PROPERTIES LINKER_LANGUAGE C)
