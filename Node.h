#pragma once

#include "Camera.h"

#include <GL/glew.h>
#include <glm/ext/matrix_float4x4.hpp>

#include <optional>
#include <array>
#include <string>

struct mesh_buffer_t {
  GLuint vertexArrayID = -1;

  struct vertexAttributeBuffer_t {
    GLuint positionBufferID = -1;
    GLuint normalBufferID = -1;
  } vertexAttribute;

  size_t count;

  struct element_t {
    GLuint elementBufferID = -1;
    int mode = GL_TRIANGLES; // default
    int componentType;       // float, unsigned short ...
    size_t count;
    size_t offset;
  } element;

  struct material_properties_t {
    enum class alphaMode_t { opaque, mask, blend };

    enum class textureKind { normalTexture, occlusionTexture, emissionTexture, baseColorTexture, metallicRoughnessTexture };

    alphaMode_t alphaMode = alphaMode_t::opaque; // default
    double alphaCutoff = 0.5;                    // default
    bool doubleSided = false;                    // default

    std::array<float, 4> baseColorFactor = {1.0, 1.0, 1.0, 1.0}; // default
    float roughnessFactor = 1.0;                                 // default
    float metallicFactor = 1.0;                                  // default

    GLuint normalTextureID = -1;
    GLuint occlusionTextureID = -1;
    GLuint emissionTextureID = -1;
    GLuint baseColorTextureID = -1;
    GLuint metallicRoughnessTextureID = -1;

    std::unordered_map<std::string, GLuint> textureUV;

  } material;
};

struct node_t {
  enum class type_t { mesh, camera, skin };
  type_t type;

  mesh_buffer_t mesh_buffer;

  std::optional<Camera> camera;

  glm::mat4x4 transformMatrix() const {
    return transformMatrix_;
  }

  glm::mat4x4 transformMatrix_ = glm::mat4x4(1.0f);
};
