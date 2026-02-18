#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct Camera {
  Camera();

  enum class direction { up, right, down, left, front, back } dir;

  float delta = 0.0;

  glm::vec3 eye;

  glm::mat4x4 view;
  glm::mat4x4 projection;

  void update(double dT);
  void moveAround(direction dir);

  const float* projectionMatrix() const;
  const float* viewMatrix() const;
};
