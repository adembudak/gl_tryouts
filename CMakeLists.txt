cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(vibe main.cpp)

target_link_libraries(vibe PRIVATE OpenGL::GL glfw)
