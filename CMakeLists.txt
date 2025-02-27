cmake_minimum_required(VERSION 3.30)

project(GL_tryouts CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

option(VERIFY_SHADERS "Run Glslang on shader files" ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(range-v3 REQUIRED)
find_package(mpark_patterns REQUIRED)

add_library(AppBase)
target_sources(AppBase PRIVATE AppBase.cpp)
target_link_libraries(AppBase PUBLIC glfw OpenGL::GL GLEW::GLEW glm::glm
                                     range-v3::range-v3 mpark_patterns)

add_library(KTX_Loader)
target_sources(KTX_Loader PRIVATE KTX_Loader.cpp)
target_link_libraries(KTX_Loader PUBLIC OpenGL::GL GLEW::GLEW)

add_executable(vibe)
target_sources(vibe PRIVATE main.cpp)
target_link_libraries(vibe PRIVATE AppBase)

file(
  GLOB shaders
  LIST_DIRECTORIES true
  CONFIGURE_DEPENDS "shaders/*.vert" "shaders/*.frag")

add_custom_command(
  TARGET vibe
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy ${shaders} "$<TARGET_FILE_DIR:vibe>")

if(VERIFY_SHADERS)
  find_program(GLSLANG glslang glslangValidator REQUIRED)
  add_custom_command(
    TARGET vibe
    POST_BUILD
    COMMAND ${GLSLANG} ARGS --error-column ${shaders})
endif()
