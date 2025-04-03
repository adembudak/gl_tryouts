#include "Camera.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vec_swizzle.hpp>

void Camera::update(double dT) {
  delta = dT;
}

glm::mat4x4 Camera::projectionMatrix() const {
  return projection;
}

glm::mat4x4 Camera::viewMatrix() const {
  return yawPitchRoll * glm::lookAt(eye, glm::vec3(xy(eye), eye.z - 5.0f), Y_up);
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
}

void Camera::lookAround(float angleX, float angleY, float angleZ) {
  yawPitchRoll = glm::yawPitchRoll(angleX, angleY, 0.0f);
}
