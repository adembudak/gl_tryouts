#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

inline constexpr auto pi = glm::pi<float>();

inline constexpr float fieldOfView = pi / 2.0f;
inline constexpr float aspectRatio = 1.333f;
inline constexpr float zNear = 0.1f;
inline constexpr float zFar = 1000.0f;
inline constexpr glm::vec3 Y_up{0.0, 1.0, 0.0}; // Camera orientation
                                                //
struct Camera {
  glm::vec3 eye{0.0, 0.0, 5.0};

  enum class direction { left, right, front, back } dir;

  float delta = 0.0;

  glm::mat4x4 yawPitchRoll = glm::mat4x4(1);
  glm::mat4x4 projection = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);

  void lookAround(float angleX, float angleY, float angleZ);
  void moveAround(direction dir);

  void update(double dT);

  glm::mat4x4 viewMatrix() const;
  glm::mat4x4 projectionMatrix() const;
};
