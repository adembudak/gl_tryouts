#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct Camera {
  Camera();

  glm::vec3 eye{0.0, 0.0, 5.0};

  enum class direction { left, right, front, back } dir;

  float delta = 0.0;

  glm::mat4x4 yawPitchRoll;
  glm::mat4x4 projection;

  void update(double dT);
  void moveAround(direction dir);

  const float* viewMatrix();
  const float* projectionMatrix() const;
};
