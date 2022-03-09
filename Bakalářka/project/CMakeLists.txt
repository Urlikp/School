cmake_minimum_required(VERSION 3.22)
project(project)

set(CMAKE_CXX_STANDARD 14)

add_executable(project main.cpp)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
)
