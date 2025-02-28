#include "Model.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

Model& Model::load(const std::vector<glm::vec3>& vertexData, const std::vector<GLuint>& indices,
                   const std::vector<glm::vec2>& textureCoords) {
  indiceSize = indices.size();
  glCreateVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  loadVertexPositions(vertexData);
  loadTexturePositions(textureCoords);
  loadIndices(indices);

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
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLsizeiptr sizeOfVertex = sizeof(decltype(vertexPositions[0]));
  GLsizeiptr sizeOfVertices = std::size(vertexPositions) * sizeOfVertex;
  glNamedBufferStorage(arrayBufferID, sizeOfVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(arrayBufferID, 0, sizeOfVertices, std::data(vertexPositions));

  glVertexAttribPointer(0, vertexPositions[0].length(), GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
  glEnableVertexAttribArray(0);

  return *this;
}

Model& Model::loadTexturePositions(const std::vector<glm::vec2>& textureCoords) {
  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLsizeiptr sizeOfTexelElement = sizeof(decltype(textureCoords[0]));
  GLsizeiptr sizeOfTexels = std::size(textureCoords) * sizeOfTexelElement;
  glNamedBufferStorage(arrayBufferID, sizeOfTexels, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(arrayBufferID, 0, sizeOfTexels, std::data(textureCoords));

  glVertexAttribPointer(1, textureCoords[0].length(), GL_FLOAT, GL_FALSE, sizeOfTexelElement, nullptr);
  glEnableVertexAttribArray(1);

  return *this;
}

Model& Model::loadIndices(const std::vector<GLuint>& indices) {
  GLuint elementBufferID;
  glCreateBuffers(1, &elementBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

  GLsizeiptr sizeOfIndex = sizeof(decltype(indices[0]));
  GLsizeiptr sizeOfIndices = std::size(indices) * sizeOfIndex; // in bytes
  glNamedBufferStorage(elementBufferID, sizeOfIndices, std::data(indices), GL_MAP_READ_BIT);

  return *this;
}
