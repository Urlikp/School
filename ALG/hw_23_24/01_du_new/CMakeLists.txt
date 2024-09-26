cmake_minimum_required(VERSION 3.25)
project(01_du_new)

set(CMAKE_CXX_STANDARD 14)

add_executable(01_du_new main.cpp
        includes.h
        algorithm.cpp
        algorithm.h
        utils.cpp
        utils.h
        structs.h
)
