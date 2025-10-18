#include "Scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_GTX_quaternion

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

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
  for(const node_t& buffer : buffers) {
    glDeleteVertexArrays(1, &buffer.mesh_buffer.vertexArrayID);
    glDeleteBuffers(buffer.mesh_buffer.arrayBufferIDs.size(), buffer.mesh_buffer.arrayBufferIDs.data());
    glDeleteBuffers(1, &buffer.mesh_buffer.element.elementBufferID);
  }
}

void Scene::visitNode(const tn::Node& node) {
  node_t buffer;

  if(!std::empty(node.matrix) || !std::empty(node.translation) || !std::empty(node.rotation) || !std::empty(node.scale))
    loadNodeTransformData(node, buffer);

  if(int meshIndex = node.mesh; meshIndex != -1) {
    const tn::Mesh& mesh = model.meshes[meshIndex];
    visitNodeMesh(mesh, buffer.mesh_buffer);

    buffers.push_back(buffer);
  }

  else if(int cameraIndex = node.camera; cameraIndex != -1) {
    const tn::Camera& camera = model.cameras[cameraIndex];
    visitNodeCamera(camera);
  }

  else {
    return;
  }
}

void Scene::visitNodeMesh(const tn::Mesh& mesh, mesh_buffer_t& mesh_buffer) {
  glGenVertexArrays(1, &mesh_buffer.vertexArrayID);
  glBindVertexArray(mesh_buffer.vertexArrayID);

  for(const tn::Primitive& primitive : mesh.primitives)
    visitMeshPrimitive(mesh_buffer, primitive);
}

void Scene::visitNodeCamera(const tn::Camera& camera) {
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

void Scene::visitMeshPrimitive(mesh_buffer_t& buffer, const tn::Primitive& primitive) {
  for(const auto& [attribute, accessorIndex] : primitive.attributes)
    if(attribute == "POSITION")
      loadMeshVertexPositionData(buffer, accessorIndex);

  if(primitive.indices != -1) {
    buffer.element.mode = primitive.mode;
    loadMeshDrawIndices(buffer, primitive.indices);
  }
}

void Scene::loadNodeTransformData(const tn::Node& node, node_t& buffer) {
  if(!std::empty(node.matrix)) {
    buffer.transformMatrix = glm::make_mat4x4(std::data(node.matrix));
  } else {
    glm::mat4x4 T = glm::mat4x4(1.0);
    glm::mat4x4 R = glm::mat4x4(1.0);
    glm::mat4x4 S = glm::mat4x4(1.0);

    if(!std::empty(node.translation))
      T = glm::translate(glm::mat4x4(1.0), glm::vec3(glm::make_vec3(std::data(node.translation))));

    if(!std::empty(node.rotation))
      R = glm::toMat4(glm::make_quat(std::data(node.rotation)));

    if(!std::empty(node.scale))
      S = glm::scale(glm::mat4x4(1.0), glm::vec3(glm::make_vec3(std::data(node.scale))));

    buffer.transformMatrix = T * R * S;
  }
}

void Scene::loadMeshVertexPositionData(mesh_buffer_t& buffer, int accessorIndex) {
  GLuint attribIndex = glGetAttribLocation(programID, "vertexPosition");

  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);
  glBindBuffer(bv.target, arrayBufferID);

  buffer.arrayBufferIDs.push_back(arrayBufferID);

  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
  glVertexArrayVertexBuffer(buffer.vertexArrayID, attribIndex, arrayBufferID, accessor.byteOffset, accessor.ByteStride(bv));
  glVertexArrayAttribFormat(buffer.vertexArrayID, attribIndex, tn::GetNumComponentsInType(accessor.type), accessor.componentType, accessor.normalized, accessor.byteOffset);
  glEnableVertexArrayAttrib(buffer.vertexArrayID, attribIndex);

  if(accessor.sparse.isSparse) {
    const tn::Accessor::Sparse& sparse = accessor.sparse;

    const tn::BufferView& indicesBufferView = model.bufferViews[sparse.indices.bufferView];
    const tn::Buffer& indicesBuffer = model.buffers[indicesBufferView.buffer];

    const auto indices_begin = reinterpret_cast<unsigned short*>(const_cast<unsigned char*>(std::data(indicesBuffer.data)) + indicesBufferView.byteOffset + sparse.indices.byteOffset);
    const auto indices_end = indices_begin + sparse.count;
    const std::vector<unsigned short> indices(indices_begin, indices_end);

    const tn::BufferView& valuesBufferView = model.bufferViews[sparse.values.bufferView];
    const tn::Buffer& valuesBuffer = model.buffers[valuesBufferView.buffer];

    const auto values_begin = reinterpret_cast<glm::vec3*>(const_cast<unsigned char*>(std::data(valuesBuffer.data)) + valuesBufferView.byteOffset + sparse.values.byteOffset);
    const auto values_end = values_begin + sparse.count;
    const std::vector<glm::vec3> values(values_begin, values_end);

    glm::vec3* const ptr = reinterpret_cast<glm::vec3*>(glMapNamedBuffer(arrayBufferID, GL_READ_WRITE));
    for(int i = 0; i < sparse.count; ++i)
      *(ptr + indices[i]) = values[i];
    glUnmapNamedBuffer(arrayBufferID);
  }
}

void Scene::loadMeshDrawIndices(mesh_buffer_t& buffer, int accessorIndex) {
  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint elementBufferID;
  glCreateBuffers(1, &elementBufferID);
  glBindBuffer(bv.target, elementBufferID);
  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT);

  buffer.element.elementBufferID = elementBufferID;
  buffer.element.componentType = accessor.componentType;
  buffer.element.count = accessor.count;
}

bool Scene::deleteArrayBuffer(GLuint id) const {
  if(!glIsBuffer(id))
    return false;

  glDeleteBuffers(1, &id);
  return true;
}

bool Scene::deleteVertexArrayBuffer(GLuint id) const {
  if(!glIsVertexArray(id))
    return false;

  glDeleteVertexArrays(1, &id);
  return true;
}
