cmake_minimum_required(VERSION 3.3)
project(Storage)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(storage STATIC ${SRC})
set_target_properties(storage PROPERTIES LINKER_LANGUAGE C)
