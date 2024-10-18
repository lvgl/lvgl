cmake_minimum_required(VERSION 3.10)
project(rnd_unicodes C)

set(CMAKE_C_STANDARD 11)

add_library(rnd_unicodes SHARED src/library.c)

add_executable(rnd_unicodes_test examples/main.c)
target_link_libraries(rnd_unicodes_test rnd_unicodes)
