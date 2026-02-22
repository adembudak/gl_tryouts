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
    int mode;          // triangle, triangle strip etc.
    int componentType; // float, unsigned short ...
    size_t count;
  } element;
};

struct node_t {
  mesh_buffer_t mesh_buffer;

  std::optional<Camera> camera;

  glm::mat4x4 transformMatrix() const {
    return transformMatrix_;
  }

  glm::mat4x4 transformMatrix_ = glm::mat4x4(1.0f);
};
