cmake_minimum_required(VERSION 3.10)
project(conways_game_of_life C)

set(CMAKE_C_STANDARD 99)

# 只包含 main.c（其他文件等合并后再加）
add_executable(conways_game_of_life
        main.c
)

target_link_libraries(conways_game_of_life gdi32 winmm)