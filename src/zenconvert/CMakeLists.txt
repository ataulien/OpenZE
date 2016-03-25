cmake_minimum_required(VERSION 3.3)
project(Zenconvert)

file(GLOB SRC
    *.cpp
    *.h
)


add_library(zenconvert STATIC ${SRC})
target_link_libraries(zenconvert utils vdfs squish) 
set_target_properties(zenconvert PROPERTIES LINKER_LANGUAGE C)
