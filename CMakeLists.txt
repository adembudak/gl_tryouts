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

add_executable(vibe)
target_sources(vibe
  PRIVATE
    main.cpp
    Camera.cpp
    Scene.cpp
    ShaderLoader.cpp
    AppBase.cpp
    App.cpp
  PRIVATE FILE_SET HEADERS FILES
    Camera.h
    Scene.h
    Node.h
    ShaderLoader.h
    AppBase.h
    App.h)

target_link_libraries(vibe
  PRIVATE
   $<IF:$<TARGET_EXISTS:GLEW::GLEW>,GLEW::GLEW,glew::glew>
   OpenGL::GL
   glfw
   glm::glm
   tinygltf::tinygltf
   range-v3::range-v3
   mpark_patterns)

target_compile_definitions(vibe PRIVATE GLM_FORCE_MESSAGES GLM_FORCE_CXX20)

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
