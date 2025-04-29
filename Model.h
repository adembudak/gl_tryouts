#pragma once

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <tiny_gltf.h>

#include <vector>
#include <array>
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

struct Model {
  tn::Model model;

  glm::mat4x4 transform = glm::mat4(1.0);
  GLuint transformMatrixLocation;

  std::size_t indiceSize = 0;
  GLuint programID;

  float rotate_ = 0;

  void load(const std::filesystem::path& modelFile);
  void unload();

  void setProgramID(GLuint programID);

  std::vector<buffer_t> buffers;

  const std::vector<buffer_t>& getBuffers() const {
    return buffers;
  }

  void scale(const glm::vec3& v);
  void rotate(const float amount, const glm::vec3& around);
  void translate(const glm::vec3& v);

  static void switchMeshMode();

private:
  void loadModelPositionData(buffer_t& buffer, int accessorIndex);
  void loadModelDrawIndices(buffer_t& buffer, int accessorIndex);

  GLuint createArrayBuffer(int target) const;
  bool deleteArrayBuffer(GLuint id) const;

  GLuint createVertexArrayBuffer() const;
  bool deleteVertexArrayBuffer(GLuint id) const;

  void visitNode(const tn::Node& node);
  void visitMesh(const tn::Mesh& mesh);
  void visitPrimitive(buffer_t& buffer, const tn::Primitive& primitive);

  static constexpr std::array<GLenum, 3> mode = {GL_POINT, GL_LINE, GL_FILL};
};
