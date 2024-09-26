cmake_minimum_required(VERSION 3.25)
project(03_du)

set(CMAKE_CXX_STANDARD 14)

add_executable(03_du main.cpp
        utils.cpp
        utils.h
        includes.h
        structs.h
        algorithm.cpp
        algorithm.h
)
