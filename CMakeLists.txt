cmake_minimum_required(VERSION 3.25)
project(opengl CXX)

find_package(OpenGL REQUIRED)
find_package(FreeGLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLU REQUIRED)

add_executable(out main.cpp)
target_link_libraries(out PRIVATE OpenGL::OpenGL OpenGL::GLU
                                  FreeGLUT::freeglut_static GLEW::glew)
