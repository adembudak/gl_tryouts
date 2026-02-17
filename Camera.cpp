#include "Camera.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vec_swizzle.hpp>

#if !defined(NDEBUG)
  #include <glm/gtx/string_cast.hpp>
  #include <iostream>
#endif

constexpr float fieldOfView = glm::radians(90.0f);
constexpr float aspectRatio = 1.333f;
constexpr float zNear = 0.1f;
constexpr float zFar = 1000.0f;

constexpr glm::vec3 Y_up{0.0, 1.0, 0.0}; // Camera orientation
constexpr glm::vec3 center{0.0, 0.0, 0.0};

Camera::Camera() {
  eye = glm::vec3{0.0, 0.0, 5.0};
  view = glm::lookAt(eye, center, Y_up);
  projection = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

void Camera::update(double dT) {
  delta = dT;
}

void Camera::moveAround(direction dir) {
  float speed = delta * 2.0;
  switch(dir) {
    using enum direction;
  case left:  eye += speed * glm::normalize(cross({0.0, 0.0, -5.0}, Y_up)); break;
  case right: eye += speed * glm::normalize(cross(Y_up, {0.0, 0.0, -5.0})); break;
  case front: eye += (speed * glm::vec3(0.0, 0.0, -5.0)); break;
  case back:  eye += (-speed * glm::vec3(0.0, 0.0, -5.0)); break;
  }

#if !defined(NDEBUG)
  std::cout << glm::to_string(eye) << '\n';
#endif
  view = glm::lookAt(eye, glm::vec3(xy(eye), eye.z - 5.0f), Y_up);
}

const float* Camera::viewMatrix() const {
  return glm::value_ptr(view);
}

const float* Camera::projectionMatrix() const {
  return glm::value_ptr(projection);
}
