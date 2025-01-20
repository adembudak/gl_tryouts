cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

option(VERIFY_SHADERS "Run Glslang on shader files" ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(range-v3 REQUIRED)
find_package(mpark_patterns REQUIRED)

add_library(shaderLoader)
target_sources(shaderLoader PRIVATE shaderLoader.cpp)
target_sources(shaderLoader PRIVATE FILE_SET HEADERS BASE_DIRS
                                    ${PROJECT_SOURCE_DIR} FILES shaderLoader.h)
target_link_libraries(shaderLoader PRIVATE OpenGL::GL range-v3::range-v3
                                           GLEW::GLEW mpark_patterns)

add_executable(vibe)
target_sources(vibe PRIVATE main.cpp)
target_link_libraries(vibe PRIVATE OpenGL::GL glfw GLEW::GLEW shaderLoader)

file(
  GLOB shaders
  LIST_DIRECTORIES true
  CONFIGURE_DEPENDS "shaders/*")

if(VERIFY_SHADERS)
  find_program(GLSLANG glslang glslangValidator REQUIRED)
  add_custom_command(
    TARGET vibe
    POST_BUILD
    COMMAND ${GLSLANG} ARGS ${shaders})
endif()

add_custom_command(
  TARGET vibe
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy ${shaders} "$<TARGET_FILE_DIR:vibe>")
