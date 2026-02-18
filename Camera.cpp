#include "Camera.h"

#define GLM_FORCE_XYZW_ONLY

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtc/type_ptr.hpp>

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

constexpr glm::vec3 defaultCameraPosition{0.0, 0.0, 5.0};

Camera::Camera() {
  eye = defaultCameraPosition;

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

  case up:    eye += speed * Y_up; break;
  case right: eye += speed * glm::normalize(cross(-defaultCameraPosition, Y_up)); break;
  case down:  eye += speed * -Y_up; break;
  case left:  eye += speed * glm::normalize(cross(Y_up, -defaultCameraPosition)); break;
  case front: eye += (speed * -defaultCameraPosition); break;
  case back:  eye += (-speed * -defaultCameraPosition); break;
  }

#if !defined(NDEBUG)
  std::cout << glm::to_string(eye) << '\n';
#endif
  view = glm::lookAt(eye, glm::vec3(xy(eye), eye.z - 5.0f), Y_up);
}

const float* Camera::projectionMatrix() const {
  return glm::value_ptr(projection);
}

const float* Camera::viewMatrix() const {
  return glm::value_ptr(view);
}
