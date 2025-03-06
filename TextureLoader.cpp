#include "TextureLoader.h"

#include <cassert>
#include <filesystem>

#include <ktx.h>

namespace util {

bool TextureLoader::load(const std::filesystem::path& textureFile) {
  assert(std::filesystem::exists(textureFile));

  ktxTexture* kTexture;
  KTX_error_code ret = ktxTexture_CreateFromNamedFile(textureFile.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);

  glGenTextures(1, &textureID);
  ret = ktxTexture_GLUpload(kTexture, &textureID, &target, nullptr);

  return true;
}

}
