#include "shaderLoader.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <range/v3/all.hpp>
#include <mpark/patterns.hpp>

#include <fstream>
#include <string>
#include <filesystem>

namespace util {

shaderLoader& shaderLoader::load(const std::vector<std::filesystem::path>& shaderFiles) {
  for(auto shaderFile : shaderFiles) {
    std::ifstream fin{shaderFile};
    fin.unsetf(std::ifstream::skipws);

    std::string shaderSource = ranges::istream<char>(fin) | ranges::to<std::string>;

    auto data = std::data(shaderSource);
    auto size = std::size(shaderSource);

    GLenum type = this->identifyShaderType(shaderFile);
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &data, nullptr);

    shaderIDs.push_back(id);
  }

  return *this;
}

shaderLoader& shaderLoader::compile() {
  for(auto id : shaderIDs)
    glCompileShader(id);

  return *this;
}

shaderLoader& shaderLoader::attach() {
  programID = glCreateProgram();

  for(auto shaderID : shaderIDs)
    glAttachShader(programID, shaderID);

  return *this;
}

shaderLoader& shaderLoader::link() {
  glLinkProgram(programID);

  return *this;
}

GLenum shaderLoader::identifyShaderType(const std::filesystem::path shaderFile) const {
  using namespace mpark::patterns;
  // Based on: https://github.com/KhronosGroup/glslang?tab=readme-ov-file#execution-of-standalone-wrapper
  // clang-format off
  return match(shaderFile.extension().string())(
      pattern(".vert") = [] { return GL_VERTEX_SHADER; },         
      pattern(".tesc") = [] { return GL_TESS_CONTROL_SHADER; },   
      pattern(".tese") = [] { return GL_TESS_EVALUATION_SHADER; },
      pattern(".geom") = [] { return GL_GEOMETRY_SHADER; },       
      pattern(".frag") = [] { return GL_FRAGMENT_SHADER; },       
      pattern(".comp") = [] { return GL_COMPUTE_SHADER; }         
  );
  // clang-format on
}

}
