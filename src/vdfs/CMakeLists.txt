cmake_minimum_required(VERSION 3.3)
project(VDFS)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(vdfs STATIC ${SRC})
add_executable(vdfs_test ${SRC})
set_target_properties(vdfs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(vdfs_test utils)