cmake_minimum_required(VERSION 3.3)
project(GameClasses)

file(GLOB SRC
    *.cpp
    *.h
)

add_library(game_classes STATIC ${SRC})
set_target_properties(game_classes PROPERTIES LINKER_LANGUAGE C)
