cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(range-v3 REQUIRED)
find_package(mpark_patterns REQUIRED)

add_library(shaderLoader)
target_sources(shaderLoader PRIVATE shaderLoader.cpp)
target_link_libraries(shaderLoader PRIVATE OpenGL::GL range-v3::range-v3
                                           GLEW::GLEW mpark_patterns)

add_executable(vibe)
target_sources(vibe PRIVATE main.cpp)
target_link_libraries(vibe PRIVATE OpenGL::GL glfw GLEW::GLEW shaderLoader)
