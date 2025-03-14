#include "ShaderLoader.h"

#include <GL/glew.h>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/istream.hpp>
#include <mpark/patterns/match.hpp>

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace util {

ShaderLoader& ShaderLoader::load(const std::vector<std::filesystem::path>& shaderFiles) {
  for(auto shaderFile : shaderFiles) {
    std::string shaderSource = this->getShaderFileSource(shaderFile);
    auto data = std::data(shaderSource);

    GLenum type = this->identifyShaderType(shaderFile);

    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &data, nullptr);
    shaderIDs.push_back(shaderID);
  }

  return *this;
}

ShaderLoader& ShaderLoader::compile() {
  for(GLuint shaderID : shaderIDs)
    glCompileShader(shaderID);

  return *this;
}

ShaderLoader& ShaderLoader::attach() {
  programID = glCreateProgram();

  for(GLuint shaderID : shaderIDs)
    glAttachShader(programID, shaderID);

  return *this;
}

ShaderLoader& ShaderLoader::link() {
  glLinkProgram(programID);

  return *this;
}

void ShaderLoader::emitProgramBinary() const {
  if(!glIsProgram(programID))
    return;

  GLint binarySize;
  glGetProgramiv(programID, GL_PROGRAM_BINARY_LENGTH, &binarySize);

  std::vector<char> programBinary(binarySize, '\0');

  GLenum binaryFormat = GL_NONE;
  glGetProgramBinary(programID, binarySize, nullptr, &binaryFormat, std::data(programBinary));

  std::ofstream fout{"programBinary.bin", std::ios::binary};
  fout.write(std::data(programBinary), std::size(programBinary));
}

GLenum ShaderLoader::identifyShaderType(const std::filesystem::path shaderFile) const {
  using namespace mpark::patterns;
  // Based on: https://github.com/KhronosGroup/glslang?tab=readme-ov-file#execution-of-standalone-wrapper
  // clang-format off
  return match(shaderFile.extension())(
      pattern(".vert") = [] { return GL_VERTEX_SHADER; },
      pattern(".tesc") = [] { return GL_TESS_CONTROL_SHADER; },
      pattern(".tese") = [] { return GL_TESS_EVALUATION_SHADER; },
      pattern(".geom") = [] { return GL_GEOMETRY_SHADER; },
      pattern(".frag") = [] { return GL_FRAGMENT_SHADER; },
      pattern(".comp") = [] { return GL_COMPUTE_SHADER; }
  );
  // clang-format on
}

std::string ShaderLoader::getShaderFileSource(const std::filesystem::path shaderFile) const {
  std::ifstream fin{shaderFile};
  fin.unsetf(std::ifstream::skipws);

  return ranges::istream<char>(fin) | ranges::to<std::string>;
}
}
