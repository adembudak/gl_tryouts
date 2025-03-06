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
find_package(Ktx REQUIRED)
find_package(TinyGLTF REQUIRED)
find_package(range-v3 REQUIRED)
find_package(mpark_patterns REQUIRED)

add_library(AppBase)
target_sources(AppBase PRIVATE AppBase.cpp)
target_link_libraries(AppBase PUBLIC glfw OpenGL::GL GLEW::GLEW glm::glm)

add_library(Model)
target_sources(Model PRIVATE Model.cpp)
target_link_libraries(Model PRIVATE OpenGL::GL GLEW::GLEW glm::glm
                                    tinygltf::tinygltf)

add_library(ShaderLoader)
target_sources(ShaderLoader PRIVATE ShaderLoader.cpp)
target_link_libraries(ShaderLoader PRIVATE OpenGL::GL GLEW::GLEW
                                           range-v3::range-v3 mpark_patterns)

add_library(TextureLoader)
target_sources(TextureLoader PRIVATE TextureLoader.cpp)
target_link_libraries(TextureLoader PRIVATE OpenGL::GL GLEW::GLEW KTX::ktx)

add_executable(vibe)
target_sources(vibe PRIVATE main.cpp)
target_link_libraries(vibe PRIVATE AppBase Model ShaderLoader TextureLoader)

set(textures ${PROJECT_SOURCE_DIR}/textures)
set(shaders ${PROJECT_SOURCE_DIR}/shaders)

add_custom_command(
  TARGET vibe
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_directory ${shaders}
          "$<PATH:APPEND,$<TARGET_FILE_DIR:vibe>,shaders>"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory ${textures}
          "$<PATH:APPEND,$<TARGET_FILE_DIR:vibe>,textures>")

if(VERIFY_SHADERS)
  file(
    GLOB shaders
    LIST_DIRECTORIES true
    CONFIGURE_DEPENDS "shaders/*.vert" "shaders/*.frag")

  find_program(GLSLANG glslang glslangValidator REQUIRED)
  add_custom_command(
    TARGET vibe
    POST_BUILD
    COMMAND ${GLSLANG} ARGS --error-column ${shaders})
endif()
