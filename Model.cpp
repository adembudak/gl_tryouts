#include "Model.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

void Model::setProgramID(GLuint programID) {
  m_programID = programID;
}

Model& Model::load(const std::vector<glm::vec3>& vertexData, const std::vector<GLuint>& indices,
                   const std::vector<glm::vec2>& textureCoords) {
  indiceSize = indices.size();
  glCreateVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  loadVertexPositions(vertexData);
  loadTexturePositions(textureCoords);
  loadDrawIndices(indices);

  return *this;
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

Model& Model::loadVertexPositions(const std::vector<glm::vec3>& vertexPositions) {
  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);

  GLsizeiptr sizeOfVertex = sizeof(decltype(vertexPositions[0]));
  GLsizeiptr sizeOfVertices = std::size(vertexPositions) * sizeOfVertex;
  glNamedBufferStorage(arrayBufferID, sizeOfVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(arrayBufferID, 0, sizeOfVertices, std::data(vertexPositions));

  GLint vPosition = glGetAttribLocation(m_programID, "vPosition"); // binding index

  glVertexArrayVertexBuffer(vertexArrayID, vPosition, arrayBufferID, 0, sizeOfVertex);
  glVertexArrayAttribFormat(vertexArrayID, vPosition, vertexPositions[0].length(), GL_FLOAT, GL_FALSE, 0);
  glEnableVertexArrayAttrib(vertexArrayID, vPosition);

  return *this;
}

Model& Model::loadTexturePositions(const std::vector<glm::vec2>& textureCoords) {
  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);

  GLsizeiptr sizeOfTexelElement = sizeof(decltype(textureCoords[0]));
  GLsizeiptr sizeOfTexels = std::size(textureCoords) * sizeOfTexelElement;
  glNamedBufferStorage(arrayBufferID, sizeOfTexels, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(arrayBufferID, 0, sizeOfTexels, std::data(textureCoords));

  GLint tPosition = glGetAttribLocation(m_programID, "tPosition");

  glVertexArrayVertexBuffer(vertexArrayID, tPosition, arrayBufferID, 0, sizeOfTexelElement);
  glVertexArrayAttribFormat(vertexArrayID, tPosition, textureCoords[0].length(), GL_FLOAT, GL_FALSE, 0);
  glEnableVertexArrayAttrib(vertexArrayID, tPosition);

  return *this;
}

Model& Model::loadDrawIndices(const std::vector<GLuint>& indices) {
  GLuint elementBufferID;
  glCreateBuffers(1, &elementBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

  GLsizeiptr sizeOfIndex = sizeof(decltype(indices[0]));
  GLsizeiptr sizeOfIndices = std::size(indices) * sizeOfIndex; // in bytes
  glNamedBufferStorage(elementBufferID, sizeOfIndices, std::data(indices), GL_MAP_READ_BIT);

  return *this;
}

void Model::switchMeshMode() {
  static std::uint8_t i = 0;
  ++i;
  i = i % std::size(mode);
  glPolygonMode(GL_FRONT_AND_BACK, mode[i]);
}
