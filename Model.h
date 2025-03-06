#pragma once

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <array>

struct Model {
  glm::mat4x4 transform = glm::mat4(1.0);
  GLuint transformMatrixLocation;

  std::size_t indiceSize = 0;

  GLuint vertexArrayID;

  float rotate_ = 0;

  // void load(std::filesystem::path& modelFile);

  Model& load(const std::vector<glm::vec3>& vertexData, const std::vector<GLuint>& indices,
              const std::vector<glm::vec2>& textureCoords);

  GLuint getVertexArrayID() const {
    return vertexArrayID;
  }

  void scale(const glm::vec3& v);
  void rotate(const float amount, const glm::vec3& around);
  void translate(const glm::vec3& v);

  static void switchMeshMode();

private:
  Model& loadVertexPositions(const std::vector<glm::vec3>& vertexPositions);
  Model& loadTexturePositions(const std::vector<glm::vec2>& textureCoords);
  Model& loadIndices(const std::vector<GLuint>& indices);

  static constexpr std::array<GLenum, 3> mode = {GL_POINT, GL_LINE, GL_FILL};
};
