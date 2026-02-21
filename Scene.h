#pragma once

#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <tiny_gltf.h>

#include <vector>
#include <filesystem>

#include "Node.h"

namespace tn = tinygltf;

struct Scene {
  tn::Model model;
  std::vector<node_t> buffers;

  GLuint programID;

  void load(const std::filesystem::path& modelglTFfile);
  void unload();

  void setProgramID(GLuint programID);

  const std::vector<node_t>& getBuffers() const {
    return buffers;
  }

private:
  void visitScene(const tn::Scene& scene);
  void visitNode(const tn::Node& node, const glm::mat4x4& parentNodeTransform);
  void visitNodeMesh(const tn::Mesh& mesh, mesh_buffer_t& mesh_buffer);
  void visitNodeCamera(const tn::Camera& camera, node_t& buffer, const glm::mat4x4& parentNodeTransform);
  void visitMeshPrimitive(mesh_buffer_t& buffer, const tn::Primitive& primitive);

  void loadNodeTransformData(const tn::Node& node, node_t& buffer, const glm::mat4x4& parentNodeTransform);
  void loadMeshVertexPositionData(mesh_buffer_t& buffer, int accessorIndex);
  void loadMeshVertexNormalData(mesh_buffer_t& buffer, int accessorIndex);
  void loadMeshDrawIndices(mesh_buffer_t& buffer, int accessorIndex);

  void loadMeshMaterial(mesh_buffer_t& buffer, int materialIndex);
};
