#pragma once

#include "Camera.h"

#include <GL/glew.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <optional>
#include <array>
#include <string>

struct mesh_buffer_t {
  GLuint vertexArrayID = -1;

  struct vertexAttributeBuffer_t {
    GLuint positionBufferID = -1;
    GLuint normalBufferID = -1;
    GLuint tangentBufferID = -1;
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

    glm::vec3 emissiveFactor = {0.0f, 0.0f, 0.0f}; // default

    struct pbrMetallicRoughness_t {
      glm::vec4 baseColorFactor = {1.0, 1.0, 1.0, 1.0}; // default
      float roughnessFactor = 1.0;                      // default
      float metallicFactor = 1.0;                       // default

      struct baseColorTexture_t {
        GLuint textureID = -1;
      } baseColorTexture;

      struct metallicRoughnessTexture_t {
        GLuint textureID = -1;
      } metallicRoughnessTexture;

    } pbr;

    struct normalTexture_t {
      float scale = 1.0; // default;
      GLuint textureID = -1;
    } normalTexture;

    struct occlusionTexture_t {
      float strength = 1.0; // default
      GLuint textureID = -1;
    } occlusionTexture;

    struct emissiveTexture_t {
      GLuint textureID = -1;
    } emissiveTexture;

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
