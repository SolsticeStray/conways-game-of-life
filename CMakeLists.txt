cmake_minimum_required(VERSION 3.10)
project(conways_game_of_life C)

set(CMAKE_C_STANDARD 99)

add_executable(conways_game_of_life
        main.c
        # core.c
        # input.c
        # render.c
)

target_link_libraries(conways_game_of_life gdi32 winmm)