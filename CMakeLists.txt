cmake_minimum_required(VERSION 3.10)
project(conways_game_of_life C)

set(CMAKE_C_STANDARD 99)

# 列出所有源文件（后面大家加新文件要在这里补上！）
add_executable(conways_game_of_life
    main.c
    core.c
    input.c
    render.c
)

# Windows 下链接 EasyX 所需的库
if(WIN32)
    target_link_libraries(conways_game_of_life gdi32)
endif()
