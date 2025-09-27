#include "Scene.h"

#include <tiny_gltf.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <filesystem>

void Scene::setProgramID(GLuint programID) {
  this->programID = programID;
}

void Scene::load(const std::filesystem::path& modelglTFFile) {
  assert(std::filesystem::exists(modelglTFFile));
  tn::TinyGLTF{}.LoadASCIIFromFile(&model, nullptr, nullptr, modelglTFFile);

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

void Scene::unload() {
  for(const buffer_t& buffer : buffers) {
    glDeleteVertexArrays(1, &buffer.vertexArrayID);
    glDeleteBuffers(buffer.arrayBufferIDs.size(), buffer.arrayBufferIDs.data());
    glDeleteBuffers(1, &buffer.element.elementBufferID);
  }
}

void Scene::scale(const glm::vec3& v) {
  transformMatrix = glm::scale(transformMatrix, v);
}

void Scene::rotate(const float amount, const glm::vec3& around) {
  rotate_ -= amount;
  transformMatrix = glm::rotate(transformMatrix, rotate_, around);
}

void Scene::translate(const glm::vec3& v) {
  transformMatrix = glm::translate(transformMatrix, v);
}

void Scene::switchMeshMode() {
  switch(mode) {
  case primitive_mode_t::point: mode = line; break;
  case primitive_mode_t::line:  mode = fill; break;
  case primitive_mode_t::fill:  mode = point; break;
  }

  glPolygonMode(GL_FRONT_AND_BACK, mode);
}

void Scene::visitNode(const tn::Node& node) {
  if(int meshIndex = node.mesh; meshIndex != -1) {
    const tn::Mesh& mesh = model.meshes[meshIndex];
    visitNodeMesh(mesh);
  }

  else if(int cameraIndex = node.camera; cameraIndex != -1) {
    const tn::Camera& camera = model.cameras[cameraIndex];
    visitNodeCamera(camera);
  }

  else {
    return;
  }
}

void Scene::visitNodeMesh(const tn::Mesh& mesh) {
  buffer_t buffer;
  GLuint vertexArrayID = createVertexArrayBuffer();
  buffer.vertexArrayID = vertexArrayID;

  for(const tn::Primitive& primitive : mesh.primitives)
    visitMeshPrimitive(buffer, primitive);

  buffers.push_back(buffer);
}

void Scene::visitNodeCamera(const tn::Camera& camera) {
  // Implement this
  if(camera.type == "perspective") {
    const tn::PerspectiveCamera& perspective = camera.perspective;
  }

  else if(camera.type == "orthographic") {
    const tn::OrthographicCamera& ortho = camera.orthographic;
  }

  else {
    return;
  }
}

void Scene::visitMeshPrimitive(buffer_t& buffer, const tn::Primitive& primitive) {
  for(const auto& [attribute, accessorIndex] : primitive.attributes)
    if(attribute == "POSITION")
      loadMeshPositionData(buffer, accessorIndex);

  if(primitive.indices != -1) {
    buffer.element.mode = primitive.mode;
    loadMeshDrawIndices(buffer, primitive.indices);
  }
}

void Scene::loadMeshPositionData(buffer_t& buffer, int accessorIndex) {
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

void Scene::loadMeshDrawIndices(buffer_t& buffer, int accessorIndex) {
  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint elementBufferID = createArrayBuffer(bv.target);
  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT);

  buffer.element.elementBufferID = elementBufferID;
  buffer.element.componentType = accessor.componentType;
  buffer.element.count = accessor.count;
}

GLuint Scene::createArrayBuffer(int target) const {
  GLuint id;
  glCreateBuffers(1, &id);
  glBindBuffer(target, id);
  return id;
}

bool Scene::deleteArrayBuffer(GLuint id) const {
  if(!glIsBuffer(id))
    return false;

  glDeleteBuffers(1, &id);
  return true;
}

GLuint Scene::createVertexArrayBuffer() const {
  GLuint id;
  glGenVertexArrays(1, &id);
  glBindVertexArray(id);
  return id;
}

bool Scene::deleteVertexArrayBuffer(GLuint id) const {
  if(!glIsVertexArray(id))
    return false;

  glDeleteVertexArrays(1, &id);
  return true;
}
