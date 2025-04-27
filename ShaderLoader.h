#pragma once

#include <GL/glew.h>

#include <filesystem>
#include <string>
#include <vector>

namespace util {

struct ShaderLoader {
  std::vector<GLuint> shaderIDs;
  GLuint programID;

  ShaderLoader& load(const std::vector<std::filesystem::path>& shaderFiles);
  ShaderLoader& compile();
  ShaderLoader& attach();
  ShaderLoader& link();
  void unload();

  void emitProgramBinary() const;

  GLuint getProgramID() const {
    return programID;
  }

private:
  GLenum identifyShaderType(const std::filesystem::path& shaderFile) const;
  std::string getShaderFileSource(const std::filesystem::path& shaderFile) const;
};

}
