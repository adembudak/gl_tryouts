#pragma once

#include <GL/glew.h>

#include <cstdint>
#include <filesystem>
#include <vector>
#include <array>

namespace ktx { namespace v1_0 {
// KTX file format loader:
// Rip off from: https://github.com/openglsuperbible/sb7code/blob/master/src/sb7/sb7ktx.cpp
// Spec: https://registry.khronos.org/KTX/specs/1.0/ktxspec.v1.html
// Up to date spec: https://registry.khronos.org/KTX/specs/2.0/ktxspec.v2.html

using ubyte = std::uint8_t;
using uint32 = std::uint32_t;

struct Header_t {
  std::array<ubyte, 12> identifier;
  uint32 endianness;
  uint32 glType;
  uint32 glTypeSize;
  uint32 glFormat;
  uint32 glInternalFormat;
  uint32 glBaseInternalFormat;
  uint32 pixelWidth;
  uint32 pixelHeight;
  uint32 pixelDepth;
  uint32 nArrayElements;
  uint32 nFaces;
  uint32 nMipmapLevel;
  uint32 bytesOfKeyValueData;
};

struct ktxLoader {
  GLuint textureID;
  std::vector<char> texels;

  bool load(const std::filesystem::path& textureFile);

private:
  Header_t header;
  GLenum target;

  bool is_ktx() const;
  uint32 calculate_stride(uint32 width, uint32 pad = 4) const;
  uint32 calculate_face_size() const;
};

}}
