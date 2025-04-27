#include "Model.h"

#include <tiny_gltf.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <filesystem>

void Model::setProgramID(GLuint programID) {
  this->programID = programID;
}

void Model::load(const std::filesystem::path& modelFile) {
  tn::TinyGLTF{}.LoadASCIIFromFile(&model, nullptr, nullptr, modelFile);

  for(const tn::Scene& scene : model.scenes) {
    for(int nodeIndex : scene.nodes) {
      const tn::Node& node = model.nodes[nodeIndex];
      if(!node.children.empty()) {
        for(int nodeIndex : node.children)
          visitNode(model.nodes[nodeIndex]);
      } else {
        visitNode(node);
      }
    }
  }
}

void Model::scale(const glm::vec3& v) {
  transform = glm::scale(transform, v);
}

void Model::rotate(const float amount, const glm::vec3& around) {
  rotate_ -= amount;
  transform = glm::rotate(transform, rotate_, around);
}

void Model::translate(const glm::vec3& v) {
  transform = glm::translate(transform, v);
}

void Model::loadModelPositionData(buffer_t& buffer, int accessorIndex) {
  GLuint attribIndex = glGetAttribLocation(programID, "vPosition");

  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint arrayBufferID = createArrayBuffer(bv.target);
  buffer.arrayBufferIDs.push_back(arrayBufferID);

  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT);

  glVertexArrayVertexBuffer(buffer.vertexArrayID, attribIndex, arrayBufferID, accessor.byteOffset, accessor.ByteStride(bv));
  glVertexArrayAttribFormat(buffer.vertexArrayID, attribIndex, tn::GetNumComponentsInType(accessor.type),
                            accessor.componentType, accessor.normalized, accessor.byteOffset);
  glEnableVertexArrayAttrib(buffer.vertexArrayID, attribIndex);
}

GLuint Model::createArrayBuffer(int target) const {
  GLuint id;
  glCreateBuffers(1, &id);
  glBindBuffer(target, id);
  return id;
}

bool Model::deleteArrayBuffer(GLuint id) const {
  if(!glIsBuffer(id))
    return false;

  glDeleteBuffers(1, &id);
  return true;
}

GLuint Model::createVertexArrayBuffer() const {
  GLuint id;
  glGenVertexArrays(1, &id);
  glBindVertexArray(id);
  return id;
}

bool Model::deleteVertexArrayBuffer(GLuint id) const {
  if(!glIsVertexArray(id))
    return false;

  glDeleteVertexArrays(1, &id);
  return true;
}

void Model::visitNode(const tn::Node& node) {
  int meshIndex = node.mesh;
  // Node MAY contain a mesh:
  //  https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#geometry-overview
  if(meshIndex == -1)
    return;

  const tn::Mesh& mesh = model.meshes[meshIndex];
  visitMesh(mesh);
}

void Model::visitMesh(const tn::Mesh& mesh) {
  buffer_t buffer;
  GLuint vertexArrayID = createVertexArrayBuffer();
  buffer.vertexArrayID = vertexArrayID;

  for(const tn::Primitive& primitive : mesh.primitives)
    visitPrimitive(buffer, primitive);

  buffers.push_back(buffer);
}

void Model::visitPrimitive(buffer_t& buffer, const tn::Primitive& primitive) {
  for(const auto& [attribute, accessorIndex] : primitive.attributes)
    if(attribute == "POSITION")
      loadModelPositionData(buffer, accessorIndex);

  if(primitive.indices != -1) {
    buffer.element.mode = primitive.mode;
    loadModelDrawIndices(buffer, primitive.indices);
  }
}

void Model::loadModelDrawIndices(buffer_t& buffer, int accessorIndex) {
  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint elementBufferID = createArrayBuffer(bv.target);
  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT);

  buffer.element.elementBufferID = elementBufferID;
  buffer.element.componentType = accessor.componentType;
  buffer.element.count = accessor.count;
}

void Model::switchMeshMode() {
  static std::uint8_t i = 0;
  ++i;
  i = i % std::size(mode);
  glPolygonMode(GL_FRONT_AND_BACK, mode[i]);
}
