#define GLM_GTX_quaternion

#include "Scene.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <tiny_gltf.h>

#include <filesystem>
#include <print>

void Scene::setProgramID(GLuint programID) {
  this->programID = programID;
}

void Scene::load(const std::filesystem::path& modelglTFFile) {
  assert(std::filesystem::exists(modelglTFFile));
  std::string error, warning;
  tn::TinyGLTF{}.LoadASCIIFromFile(&model, &error, &warning, modelglTFFile);

  if(!std::empty(error))
    std::println("Error [TinyGLTF] {}", error);

  if(!std::empty(warning))
    std::println("Warning [TinyGLTF] {}", warning);

  for(const tn::Scene& scene : model.scenes)
    visitScene(scene);
}

void Scene::unload() {
  for(const node_t& buffer : buffers) {
    if(glIsBuffer(buffer.mesh_buffer.vertexArrayID))
      glDeleteVertexArrays(1, &buffer.mesh_buffer.vertexArrayID);

    if(glIsBuffer(buffer.mesh_buffer.vertexAttribute.positionBufferID))
      glDeleteBuffers(1, &buffer.mesh_buffer.vertexAttribute.positionBufferID);

    if(glIsBuffer(buffer.mesh_buffer.vertexAttribute.normalBufferID))
      glDeleteBuffers(1, &buffer.mesh_buffer.vertexAttribute.normalBufferID);

    if(glIsBuffer(buffer.mesh_buffer.vertexAttribute.textureCoordID))
      glDeleteBuffers(1, &buffer.mesh_buffer.vertexAttribute.textureCoordID);

    if(glIsBuffer(buffer.mesh_buffer.element.elementBufferID))
      glDeleteBuffers(1, &buffer.mesh_buffer.element.elementBufferID);

    if(glIsTexture(buffer.mesh_buffer.material.normalTextureID))
      glDeleteTextures(1, &buffer.mesh_buffer.material.normalTextureID);

    if(glIsTexture(buffer.mesh_buffer.material.occlusionTextureID))
      glDeleteTextures(1, &buffer.mesh_buffer.material.occlusionTextureID);

    if(glIsTexture(buffer.mesh_buffer.material.emissionTextureID))
      glDeleteTextures(1, &buffer.mesh_buffer.material.emissionTextureID);

    if(glIsTexture(buffer.mesh_buffer.material.baseColorTextureID))
      glDeleteTextures(1, &buffer.mesh_buffer.material.baseColorTextureID);

    if(glIsTexture(buffer.mesh_buffer.material.metallicRoughnessTextureID))
      glDeleteTextures(1, &buffer.mesh_buffer.material.metallicRoughnessTextureID);
  }
}

void Scene::visitScene(const tn::Scene& scene) {
  for(int nodeIndex : scene.nodes) {
    const tn::Node& node = model.nodes[nodeIndex];
    glm::mat4x4 defaultTransform = glm::mat4x4(1.0);
    visitNode(node, defaultTransform);
  }
}

void Scene::visitNode(const tn::Node& node, const glm::mat4x4& parentNodeTransform) {
  node_t buffer;

  if(!std::empty(node.matrix) || !std::empty(node.translation) || !std::empty(node.rotation) || !std::empty(node.scale)) {
    loadNodeTransformData(node, buffer, parentNodeTransform);
  }

  if(int meshIndex = node.mesh; meshIndex != -1) {
    const tn::Mesh& mesh = model.meshes[meshIndex];
    visitNodeMesh(mesh, buffer.mesh_buffer);
  }

  if(int cameraIndex = node.camera; cameraIndex != -1) {
    const tn::Camera& camera = model.cameras[cameraIndex];
    visitNodeCamera(camera, buffer, parentNodeTransform);
  }

  buffers.push_back(std::move(buffer));

  if(!empty(node.children)) {
    for(int nodeIndex : node.children)
      visitNode(model.nodes[nodeIndex], buffer.transformMatrix_);
  }
}

void Scene::visitNodeMesh(const tn::Mesh& mesh, mesh_buffer_t& mesh_buffer) {
  glGenVertexArrays(1, &mesh_buffer.vertexArrayID);
  glBindVertexArray(mesh_buffer.vertexArrayID);

  for(const tn::Primitive& primitive : mesh.primitives)
    visitMeshPrimitive(mesh_buffer, primitive);
}

void Scene::visitNodeCamera(const tn::Camera& camera, node_t& buffer, const glm::mat4x4& parentNodeTransform) {
  if(camera.type == "perspective") {
    buffer.camera = camera.perspective;
  }

  else if(camera.type == "orthographic") {
    buffer.camera = camera.orthographic;
  }

  else {
    assert(false);
  }
}

void Scene::visitMeshPrimitive(mesh_buffer_t& buffer, const tn::Primitive& primitive) {
  for(const auto& [attribute, accessorIndex] : primitive.attributes) {
    if(attribute == "POSITION")
      loadMeshVertexPositionData(buffer, accessorIndex);

    if(attribute == "NORMAL")
      loadMeshVertexNormalData(buffer, accessorIndex);

    if(attribute.starts_with("TEXCOORD")) {
      loadMeshTextureCoordinateData(buffer, accessorIndex);
    }
  }

  if(primitive.indices != -1) {
    buffer.element.mode = primitive.mode;
    loadMeshDrawIndices(buffer, primitive.indices);
  }

  if(primitive.material != -1) {
    loadMeshMaterial(buffer, primitive.material);
  }
}

void Scene::loadNodeTransformData(const tn::Node& node, node_t& buffer, const glm::mat4x4& parentNodeTransform) {
  if(!std::empty(node.matrix)) {
    buffer.transformMatrix_ = parentNodeTransform * glm::mat4x4(glm::make_mat4x4(std::data(node.matrix)));
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

    buffer.transformMatrix_ = parentNodeTransform * T * R * S;
  }
}

void Scene::loadMeshVertexPositionData(mesh_buffer_t& buffer, int accessorIndex) {
  GLuint attribIndex = glGetAttribLocation(programID, "vertexPosition");

  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint id;
  glCreateBuffers(1, &id);
  glBindBuffer(bv.target, id);

  buffer.vertexAttribute.positionBufferID = id;

  buffer.count = accessor.count;
  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
  glVertexArrayVertexBuffer(buffer.vertexArrayID, attribIndex, buffer.vertexAttribute.positionBufferID, accessor.byteOffset, accessor.ByteStride(bv));
  glVertexArrayAttribFormat(buffer.vertexArrayID, attribIndex, tn::GetNumComponentsInType(accessor.type), accessor.componentType, accessor.normalized, accessor.byteOffset);
  glVertexArrayAttribBinding(buffer.vertexArrayID, attribIndex, attribIndex);
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

    glm::vec3* const ptr = reinterpret_cast<glm::vec3*>(glMapNamedBuffer(id, GL_READ_WRITE));
    for(int i = 0; i < sparse.count; ++i)
      *(ptr + indices[i]) = values[i];
    glUnmapNamedBuffer(id);
  }

  assert(glGetError() == GL_NO_ERROR);
}

void Scene::loadMeshVertexNormalData(mesh_buffer_t& buffer, int accessorIndex) {
  GLuint attribIndex = glGetAttribLocation(programID, "vertexNormal");

  const tn::Accessor accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  GLuint id;
  glCreateBuffers(1, &id);
  glBindBuffer(bv.target, id);

  buffer.vertexAttribute.normalBufferID = id;

  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT);
  glVertexArrayVertexBuffer(buffer.vertexArrayID, attribIndex, buffer.vertexAttribute.normalBufferID, accessor.byteOffset, accessor.ByteStride(bv));
  glVertexArrayAttribFormat(buffer.vertexArrayID, attribIndex, tn::GetNumComponentsInType(accessor.type), accessor.componentType, accessor.normalized, accessor.byteOffset);
  glVertexArrayAttribBinding(buffer.vertexArrayID, attribIndex, attribIndex);
  glEnableVertexArrayAttrib(buffer.vertexArrayID, attribIndex);

  assert(glGetError() == GL_NO_ERROR);
}

void Scene::loadMeshTextureCoordinateData(mesh_buffer_t& buffer, int accessorIndex) {
  GLuint attribIndex = glGetAttribLocation(programID, "textureCoordinate_0");

  const tn::Accessor accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  glBindVertexArray(buffer.vertexArrayID);

  GLuint id;
  glCreateBuffers(1, &id);
  glBindBuffer(bv.target, id);

  buffer.vertexAttribute.textureCoordID = id;

  glBufferStorage(bv.target, bv.byteLength, std::data(buf.data) + bv.byteOffset, GL_MAP_READ_BIT);
  glVertexArrayVertexBuffer(buffer.vertexArrayID, attribIndex, id, accessor.byteOffset, accessor.ByteStride(bv));
  glVertexArrayAttribFormat(buffer.vertexArrayID, attribIndex, tn::GetNumComponentsInType(accessor.type), accessor.componentType, accessor.normalized, accessor.byteOffset);
  glVertexArrayAttribBinding(buffer.vertexArrayID, attribIndex, attribIndex);
  glEnableVertexArrayAttrib(buffer.vertexArrayID, attribIndex);

  assert(glGetError() == GL_NO_ERROR);
}

void Scene::loadMeshDrawIndices(mesh_buffer_t& buffer, int accessorIndex) {
  const tn::Accessor& accessor = model.accessors[accessorIndex];
  const tn::BufferView& bv = model.bufferViews[accessor.bufferView];
  const tn::Buffer& buf = model.buffers[bv.buffer];

  glBindVertexArray(buffer.vertexArrayID);

  GLuint id;
  glCreateBuffers(1, &id);
  glBindBuffer(bv.target, id);

  buffer.element.elementBufferID = id;

  glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, bv.byteLength, std::data(buf.data) + bv.byteOffset + accessor.byteOffset, GL_MAP_READ_BIT);

  buffer.element.componentType = accessor.componentType;
  buffer.element.count = accessor.count;

  assert(glGetError() == GL_NO_ERROR);
}

void Scene::loadMeshMaterial(mesh_buffer_t& buffer, int materialIndex) {
  const tn::Material& material = model.materials[materialIndex];

  const tn::NormalTextureInfo& normalTexture = material.normalTexture;
  if(normalTexture.index != -1) {
  }

  const tn::OcclusionTextureInfo& occlusionTexture = material.occlusionTexture;
  if(occlusionTexture.index != -1) {
  }

  const tn::TextureInfo& emissiveTexture = material.emissiveTexture;
  if(emissiveTexture.index != -1) {
  }

  const tn::PbrMetallicRoughness& pbr = material.pbrMetallicRoughness;

  buffer.material.roughnessFactor = pbr.roughnessFactor;
  ranges::copy(pbr.baseColorFactor, std::begin(buffer.material.baseColorFactor));
  buffer.material.metallicFactor = pbr.metallicFactor;

  const tn::TextureInfo& baseColorTexture = pbr.baseColorTexture;
  if(baseColorTexture.index != -1) {
    loadTexture(buffer, baseColorTexture.index, mesh_buffer_t::material_properties_t::textureKind::baseColorTexture);
  }

  const tn::TextureInfo& metallicRoughnessTexture = pbr.metallicRoughnessTexture;
  if(metallicRoughnessTexture.index != -1) {
  }
}

void Scene::loadTexture(mesh_buffer_t& buffer, int textureIndex, mesh_buffer_t::material_properties_t::textureKind kind) {
  const tn::Texture& texture = model.textures[textureIndex];
  const tn::Sampler& sampler = model.samplers[texture.sampler];
  const tn::Image& im = model.images[texture.source];

  GLuint id;
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  assert(glIsTexture(id));

  glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
  glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
  glTextureParameteri(id, GL_TEXTURE_WRAP_S, sampler.wrapS);
  glTextureParameteri(id, GL_TEXTURE_WRAP_T, sampler.wrapT);

  const unsigned char* pixels_data = nullptr;
  if(im.bufferView != -1) {
    const tn::BufferView& bv = model.bufferViews[im.bufferView];
    const tn::Buffer& buf = model.buffers[bv.buffer];
    pixels_data = std::data(buf.data) + bv.byteOffset;
  } else {
    pixels_data = std::data(im.image);
  }

  GLenum format = 0;
  switch(im.component) {
  case 1: format = GL_RED; break;
  case 2: format = GL_RG; break;
  case 3: format = GL_RGB; break;
  case 4: format = GL_RGBA; break;
  }
  assert(format != 0);

  GLenum internalFormat = 0;
  switch(kind) {
    using enum mesh_buffer_t::material_properties_t::textureKind;
  case normalTexture:
    internalFormat = im.component == 4 ? GL_RGBA8 : GL_RGB8;
    buffer.material.normalTextureID = id;
    break;

  case occlusionTexture:
    internalFormat = im.component == 4 ? GL_RGBA8 : im.component == 3 ? GL_RGB8 : im.component == 2 ? GL_RG8 : GL_R8;
    buffer.material.occlusionTextureID = id;
    break;

  case emissionTexture:
    internalFormat = im.component == 4 ? GL_RGBA8 : GL_RGB8;
    buffer.material.emissionTextureID = id;
    break;

  case baseColorTexture:
    internalFormat = im.component == 4 ? GL_SRGB8_ALPHA8 : GL_SRGB8;
    buffer.material.baseColorTextureID = id;
    break;

  case metallicRoughnessTexture:
    internalFormat = im.component == 4 ? GL_RGBA8 : GL_RGB8;
    buffer.material.metallicRoughnessTextureID = id;
    break;
  };
  assert(internalFormat != 0);

  std::print("im.component: {}\nim.bits: {}\nim.width: {}\nim.height: {}\nim.mimeType: {}\n", im.component, im.bits, im.width, im.height, im.mimeType);

  glTextureStorage2D(id, 1, internalFormat, im.width, im.height);
  glTextureSubImage2D(id, 0, 0, 0, im.width, im.height, format, im.pixel_type, pixels_data);

  assert(glGetError() == GL_NO_ERROR);
};
