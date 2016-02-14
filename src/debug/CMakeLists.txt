cmake_minimum_required(VERSION 3.3)
project(LibDebug)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(libdebug STATIC ${SRC})
set_target_properties(libdebug PROPERTIES LINKER_LANGUAGE C)
