#pragma once

// clang-format off
#include <GL/glew.h> // this include is order dependent
#include <GL/gl.h>
// clang-format on

#include <filesystem>
#include <vector>

namespace util {

struct shaderLoader {
  std::vector<GLuint> shaderIDs;

  GLuint programID;

  shaderLoader &load(const std::vector<std::filesystem::path> &shaderFiles);
  shaderLoader &compile();
  shaderLoader &attach();
  shaderLoader &link();

  GLuint getProgramID() const {
    return programID;
  }

private:
  GLenum identifyShaderType(const std::filesystem::path shaderFile) const;
};
}
