cmake_minimum_required(VERSION 3.17)
project(1boats C)

set(CMAKE_C_STANDARD 99)

add_executable(1boats main.c)
target_link_libraries(1boats m)