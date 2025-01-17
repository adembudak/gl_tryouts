cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(vibe main.cpp)

target_link_libraries(vibe PRIVATE OpenGL::GL glfw)
