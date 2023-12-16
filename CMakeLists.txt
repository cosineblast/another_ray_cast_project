
cmake_minimum_required(VERSION 3.5)

project(rcast)

add_executable(main
  src/main.c
  src/timing.c
  src/player.c
  src/map.c
  src/view.c
  src/vec.c
  src/preview.c
  src/cast.c)



target_link_libraries(main m)

find_package(SDL2 REQUIRED)

target_link_libraries(main SDL2)

target_link_libraries(main SDL2_image)

target_include_directories(main PRIVATE ${SDL2_INCLUDE_DIRS})


set(CMAKE_C_FLAGS "-Wall")

file(
  COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
