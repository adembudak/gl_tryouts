#include "Camera.h"

#define GLM_FORCE_XYZW_ONLY

#include <glm/glm.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtc/type_ptr.hpp>

#if !defined(NDEBUG)
  #include <glm/gtx/string_cast.hpp>
  #include <iostream>
#endif

#include <tiny_gltf.h>

float delta = 0.0;

Camera::Camera(const tn::PerspectiveCamera& p) {
  projection = glm::perspective(p.yfov, p.aspectRatio, p.znear, p.zfar);
}

Camera::Camera(const tn::OrthographicCamera& o) {
  projection = glm::ortho(-o.xmag, o.xmag, -o.ymag, o.ymag, o.znear, o.zfar);
}

glm::mat4x4 Camera::projectionMatrix() const {
  return projection;
}

constexpr float fieldOfView = glm::radians(90.0f);
constexpr float aspectRatio = 1.333f;
constexpr float zNear = 0.1f;
constexpr float zFar = 1000.0f;
static const glm::mat4x4 defaultProjection = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);

const glm::mat4x4& Camera::defaultPerspectiveCamera() {
  return defaultProjection;
}

constexpr glm::vec3 eye{0.0, 0.0, 5.0};
constexpr glm::vec3 center{0.0, 0.0, 0.0};
constexpr glm::vec3 Y_up{0.0, 1.0, 0.0}; // Camera orientation
static const glm::mat4x4 defaultView = glm::lookAt(eye, center, Y_up);

const glm::mat4x4& Camera::defaultCameraPosition() {
  return defaultView;
}
