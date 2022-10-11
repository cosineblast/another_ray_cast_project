
cmake_minimum_required(VERSION 3.0)

project(rcast)

add_executable(main src/main.c src/timing.c src/player.c src/map.c src/view.c)


target_link_libraries(main SDL2)

target_link_libraries(main m)
