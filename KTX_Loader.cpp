#include "KTX_Loader.h"

#include <GL/glew.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <cassert>

namespace ktx { namespace v1_0 {

constexpr std::array<ubyte, 12> identifierExpected{0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

bool ktxLoader::is_ktx() const {
  return header.identifier == identifierExpected;
}

uint32 ktxLoader::calculate_stride(uint32 width, uint32 pad) const {
  uint32 channels = 0;
  switch(header.glBaseInternalFormat) {
  case GL_RED:  channels = 1; break;
  case GL_RG:   channels = 2; break;
  case GL_BGR:
  case GL_RGB:  channels = 3; break;
  case GL_BGRA:
  case GL_RGBA: channels = 4; break;
  }

  uint32 stride = header.glTypeSize * channels * width;

  stride = (stride + (pad - 1)) & ~(pad - 1);

  return stride;
}

uint32 ktxLoader::calculate_face_size() const {
  return calculate_stride(header.pixelWidth) * header.pixelHeight;
}

bool ktxLoader::load(const std::filesystem::path& textureFile) {
  std::ifstream fin{textureFile, std::ios::binary};

  if(!fin)
    return false;

  fin.read(reinterpret_cast<char*>(&header), sizeof(decltype(header)));

  if(!is_ktx())
    return false;

  auto swap32 = [](const uint32 u32) -> uint32 {
    union {
      uint32 u32;
      ubyte u8[4];
    } a, b;

    a.u32 = u32;
    b.u8[0] = a.u8[3];
    b.u8[1] = a.u8[2];
    b.u8[2] = a.u8[1];
    b.u8[3] = a.u8[0];

    return b.u32;
  };

  if(header.endianness == 0x01020304) {
    header.endianness = swap32(header.endianness);
    header.glType = swap32(header.glType);
    header.glTypeSize = swap32(header.glTypeSize);
    header.glFormat = swap32(header.glFormat);
    header.glInternalFormat = swap32(header.glInternalFormat);
    header.glBaseInternalFormat = swap32(header.glBaseInternalFormat);
    header.pixelWidth = swap32(header.pixelWidth);
    header.pixelHeight = swap32(header.pixelHeight);
    header.pixelDepth = swap32(header.pixelDepth);
    header.nArrayElements = swap32(header.nArrayElements);
    header.nFaces = swap32(header.nFaces);
    header.nMipmapLevel = swap32(header.nMipmapLevel);
    header.bytesOfKeyValueData = swap32(header.bytesOfKeyValueData);
  } else {
    assert(header.endianness == 0x04030201);
  }

  target = GL_NONE;

  if(header.pixelHeight == 0) {
    target = (header.nArrayElements == 0) ? GL_TEXTURE_1D : GL_TEXTURE_1D_ARRAY;
  } else if(header.pixelDepth == 0) {
    if(header.nArrayElements == 0)
      target = (header.nFaces == 6) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    else
      target = (header.nFaces == 6) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_2D_ARRAY;
  } else {
    target = GL_TEXTURE_3D;
  }

  assert(target != GL_NONE);

  glCreateTextures(target, 1, &textureID);

  std::istreambuf_iterator<char> iter{fin};
  texels = std::vector<char>(next(iter, header.bytesOfKeyValueData), {});

  if(header.nMipmapLevel == 0)
    header.nMipmapLevel = 1;

  const auto& h = header;
  switch(target) {
  case GL_TEXTURE_1D:
    glTextureStorage1D(textureID, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth);
    glTextureSubImage1D(textureID, h.nMipmapLevel, 0, h.pixelWidth, h.glFormat, h.glType, std::data(texels));
    break;

  case GL_TEXTURE_1D_ARRAY:
    glTextureStorage2D(textureID, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth, h.pixelHeight);

    glTextureSubImage2D(textureID, h.nMipmapLevel, 0, 0, h.pixelWidth, h.pixelHeight, h.glFormat, h.glType,
                        std::data(texels));

    break;

  case GL_TEXTURE_2D:
    if(h.glType == GL_NONE)
      glCompressedTexImage2D(GL_TEXTURE_2D, 0, h.glInternalFormat, h.pixelWidth, h.pixelHeight, 0, std::size(texels),
                             std::data(texels));

    else {
      glTextureStorage2D(textureID, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth, h.pixelHeight);
      glTextureSubImage2D(textureID, 0, 0, 0, h.pixelWidth, h.pixelHeight, h.glFormat, h.glType, std::data(texels));

      uint32 pixelWidth = h.pixelWidth;
      uint32 pixelHeight = h.pixelHeight;

      auto ptr = std::data(texels);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      for(int i = 0; i < h.nMipmapLevel; i++) {
        glTextureSubImage2D(textureID, i, 0, 0, pixelWidth, pixelHeight, h.glFormat, h.glType, ptr);

        ptr += h.pixelHeight * calculate_stride(h.pixelWidth, 1);

        pixelHeight >>= 1;
        pixelWidth >>= 1;

        if(!pixelHeight)
          pixelHeight = 1;
        if(!pixelWidth)
          pixelWidth = 1;
      }
    }
    break;

  case GL_TEXTURE_2D_ARRAY:
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth, h.pixelHeight, h.nArrayElements);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, h.pixelWidth, h.pixelHeight, h.nArrayElements, h.glFormat, h.glType,
                    std::data(texels));

    break;

  case GL_TEXTURE_CUBE_MAP:
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth, h.pixelHeight);
    glTexSubImage3D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, h.pixelWidth, h.pixelHeight, h.nFaces, h.glFormat, h.glType,
                    std::data(texels));
    {
      int face_size = calculate_face_size();
      for(int i = 0; i < h.nFaces; i++) {
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, h.pixelWidth, h.pixelHeight, h.glFormat, h.glType,
                        std::data(texels) + face_size * i);
      }
    }
    break;
  case GL_TEXTURE_CUBE_MAP_ARRAY:
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth, h.pixelHeight,
                   h.nArrayElements);
    glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, h.pixelWidth, h.pixelHeight, h.nFaces * h.nArrayElements,
                    h.glFormat, h.glType, std::data(texels));
    break;

  case GL_TEXTURE_3D:
    glTexStorage3D(GL_TEXTURE_3D, h.nMipmapLevel, h.glInternalFormat, h.pixelWidth, h.pixelHeight, h.pixelDepth);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, h.pixelWidth, h.pixelHeight, h.pixelDepth, h.glFormat, h.glType,
                    std::data(texels));
    break;
  };

  return true;
}

}}
