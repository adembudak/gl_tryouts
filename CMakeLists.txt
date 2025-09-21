cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_EXTENSIONS 0)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

option(VERIFY_SHADERS "Run Glslang on shader files" 0)
option(VISUALIZE_TARGETS "Run Graphviz on targets to see linkage graph" 0)

include(FindGLEW)

find_package(glfw3 QUIET REQUIRED CONFIG)
find_package(OpenGL QUIET REQUIRED)
find_package(glm QUIET REQUIRED CONFIG)
find_package(TinyGLTF QUIET REQUIRED CONFIG)
find_package(range-v3 QUIET REQUIRED CONFIG)
find_package(mpark_patterns QUIET REQUIRED CONFIG)

add_library(AppBase)
target_sources(AppBase PRIVATE AppBase.cpp)
target_link_libraries(AppBase PUBLIC glfw OpenGL::GL GLEW::GLEW)

add_library(Model)
target_sources(Model PRIVATE Model.cpp)
target_link_libraries(Model PUBLIC OpenGL::GL GLEW::GLEW glm::glm
                                   tinygltf::tinygltf)

add_library(ShaderLoader)
target_sources(ShaderLoader PRIVATE ShaderLoader.cpp)
target_link_libraries(ShaderLoader PRIVATE OpenGL::GL GLEW::GLEW
                                           range-v3::range-v3 mpark_patterns)

add_library(Camera)
target_sources(Camera PRIVATE Camera.cpp)
target_link_libraries(Camera PRIVATE glm::glm)

add_library(Thing)
target_sources(Thing PRIVATE Thing.cpp)
target_link_libraries(Thing PUBLIC AppBase Model ShaderLoader Camera)

add_executable(vibe)
target_sources(vibe PRIVATE main.cpp)
target_link_libraries(vibe PRIVATE Thing)

set(shaders ${PROJECT_SOURCE_DIR}/shaders)

add_custom_command(
  TARGET vibe
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${shaders}
          "$<PATH:APPEND,$<TARGET_FILE_DIR:vibe>,shaders>")

if(VERIFY_SHADERS)
  file(
    GLOB shader_files
    LIST_DIRECTORIES true
    CONFIGURE_DEPENDS "shaders/*.vert" "shaders/*.frag")

  find_program(GLSLANG_COMMAND glslang glslangValidator REQUIRED)
  add_custom_command(
    TARGET vibe
    POST_BUILD
    COMMAND ${GLSLANG_COMMAND} ARGS --error-column ${shader_files})
endif()

if(VISUALIZE_TARGETS)
  find_program(DOT_COMMAND dot REQUIRED)

  add_custom_target(
    graphviz ALL
    COMMAND ${CMAKE_COMMAND} --graphviz=tgt.dot ${PROJECT_BINARY_DIR}
    COMMAND ${DOT_COMMAND} -Tpng tgt.dot -o targets.png
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
