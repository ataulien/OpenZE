cmake_minimum_required(VERSION 3.3)
project(Physics)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(physics STATIC ${SRC})
set_target_properties(physics PROPERTIES LINKER_LANGUAGE C)

