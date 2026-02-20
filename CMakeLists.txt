cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_EXTENSIONS 0)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

option(VERIFY_SHADERS "Run Glslang on shader files" 0)
option(VISUALIZE_TARGETS "Run Graphviz on targets to see linkage graph" 0)

find_package(glew NAMES glew GLEW QUIET CONFIG REQUIRED)
find_package(glfw3 QUIET REQUIRED CONFIG)
find_package(OpenGL QUIET REQUIRED)
find_package(glm QUIET REQUIRED CONFIG)
find_package(TinyGLTF QUIET REQUIRED CONFIG)
find_package(range-v3 QUIET REQUIRED CONFIG)
find_package(mpark_patterns QUIET REQUIRED CONFIG)

add_library(AppBase)
target_sources(AppBase PRIVATE AppBase.cpp)
target_link_libraries(AppBase PUBLIC glfw OpenGL::GL GLEW::GLEW)

add_library(Camera)
target_sources(Camera PRIVATE Camera.cpp)
target_link_libraries(Camera PRIVATE glm::glm tinygltf::tinygltf)
target_compile_definitions(Camera PRIVATE GLM_FORCE_MESSAGES GLM_FORCE_CXX20)

add_library(Scene)
target_sources(Scene PRIVATE Scene.cpp)
target_link_libraries(Scene
  PUBLIC OpenGL::GL $<IF:$<TARGET_EXISTS:GLEW::GLEW>,GLEW::GLEW,glew::glew> glm::glm tinygltf::tinygltf
  PRIVATE Camera)

add_library(ShaderLoader)
target_sources(ShaderLoader PRIVATE ShaderLoader.cpp)
target_link_libraries(ShaderLoader PRIVATE OpenGL::GL GLEW::GLEW range-v3::range-v3 mpark_patterns)

add_library(App)
target_sources(App PRIVATE App.cpp)
target_link_libraries(App PUBLIC AppBase Scene ShaderLoader Camera)

add_executable(vibe)
target_sources(vibe PRIVATE main.cpp)
target_link_libraries(vibe PRIVATE App)

cmake_path(SET shaders_path "${PROJECT_SOURCE_DIR}/shaders")

add_custom_command(TARGET vibe
  POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_newer ${shaders_path} "$<PATH:APPEND,$<TARGET_FILE_DIR:vibe>,shaders>")

if(VERIFY_SHADERS)
  file(GLOB shader_files
    LIST_DIRECTORIES true
    CONFIGURE_DEPENDS "${shaders_path}/*.vert" "${shaders_path}/*.frag")

  find_program(GLSLANG_COMMAND glslang glslangValidator REQUIRED)
  add_custom_command(TARGET vibe
    POST_BUILD COMMAND ${GLSLANG_COMMAND} ARGS --error-column ${shader_files})
endif()

if(VISUALIZE_TARGETS)
  find_program(DOT_COMMAND dot REQUIRED)

  add_custom_target(graph ALL
    COMMAND ${CMAKE_COMMAND} --graphviz=targets.dot ${PROJECT_BINARY_DIR}
    COMMAND ${DOT_COMMAND} -Tsvg targets.dot -o targets.svg
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
