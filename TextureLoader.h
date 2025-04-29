#pragma once

#include <GL/glew.h>

#include <filesystem>

namespace util {

struct TextureLoader {
  GLuint textureID;
  GLenum target;

  bool load(const std::filesystem::path& textureFile);
  void unload();

  GLuint getTextureID() const {
    return textureID;
  }
};

}
