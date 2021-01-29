cmake_minimum_required(VERSION 3.17)
project(inverse_matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(inverse_matrix main.cpp solver.cpp solver.h matrix.cpp matrix.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(inverse_matrix Threads::Threads)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(inverse_matrix PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(inverse_matrix PRIVATE /W4)
endif()
