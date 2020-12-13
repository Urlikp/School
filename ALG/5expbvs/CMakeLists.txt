cmake_minimum_required(VERSION 3.17)
project(5expbvs)

set(CMAKE_CXX_STANDARD 11)

set(GCC_COVERAGE_COMPILE_FLAGS "-pipe -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(5expbvs main.cpp)
add_executable(test test.cpp)
