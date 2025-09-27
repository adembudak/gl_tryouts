#pragma once

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <tiny_gltf.h>

#include <vector>
#include <filesystem>

namespace tn = tinygltf;

struct buffer_t {
  GLuint vertexArrayID;
  std::vector<GLuint> arrayBufferIDs;

  struct element_t {
    GLuint elementBufferID;
    int mode;
    int componentType;
    size_t count;
  } element;
};

struct Scene {
  tn::Model model;
  std::vector<buffer_t> buffers;

  glm::mat4x4 transformMatrix = glm::mat4(1.0);
  GLuint transformMatrixLocation;

  GLuint programID;

  float rotate_ = 0;

  void load(const std::filesystem::path& modelglTFfile);
  void unload();

  void setProgramID(GLuint programID);

  const std::vector<buffer_t>& getBuffers() const {
    return buffers;
  }

  void scale(const glm::vec3& v);
  void rotate(const float amount, const glm::vec3& around);
  void translate(const glm::vec3& v);

  void switchMeshMode();

private:
  void visitNode(const tn::Node& node);
  void visitNodeMesh(const tn::Mesh& mesh);
  void visitNodeCamera(const tn::Camera& camera);
  void visitMeshPrimitive(buffer_t& buffer, const tn::Primitive& primitive);

  void loadMeshPositionData(buffer_t& buffer, int accessorIndex);
  void loadMeshDrawIndices(buffer_t& buffer, int accessorIndex);

  GLuint createArrayBuffer(int target) const;
  bool deleteArrayBuffer(GLuint id) const;

  GLuint createVertexArrayBuffer() const;
  bool deleteVertexArrayBuffer(GLuint id) const;

  enum primitive_mode_t : GLenum { point = GL_POINT, line = GL_LINE, fill = GL_FILL };
  primitive_mode_t mode = primitive_mode_t::fill;
};
