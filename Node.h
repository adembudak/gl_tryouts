#pragma once
#include "Camera.h"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include <optional>

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
  } element;

  struct material_properties_t {
    enum class alphaMode_t { opaque, mask, blend };

    alphaMode_t alphaMode = alphaMode_t::opaque; // default
    double alphaCutoff = 0.5;                    // default
    bool doubleSided = false;                    // default

    std::array<double, 4> baseColorFactor = {1.0, 1.0, 1.0, 1.0}; // default
    double roughnessFactor = 1.0;                                 // default
    double metallicFactor = 1.0;                                  // default
  } material;
};

struct node_t {
  mesh_buffer_t mesh_buffer;

  std::optional<Camera> camera;

  glm::mat4x4 transformMatrix() const {
    return transformMatrix_;
  }

  glm::mat4x4 transformMatrix_ = glm::mat4x4(1.0f);
};
