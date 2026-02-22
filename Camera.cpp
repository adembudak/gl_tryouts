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

static glm::mat4x4 defaultProjection;
static glm::mat4x4 defaultView;

glm::mat4x4 Camera::defaultPerspectiveCamera() {
  constexpr float fieldOfView = glm::radians(90.0f);
  constexpr float aspectRatio = 1.333f;
  constexpr float zNear = 0.1f;
  constexpr float zFar = 1000.0f;

  defaultProjection = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
  return defaultProjection;
}

glm::mat4x4 Camera::defaultCameraPosition() {
  constexpr glm::vec3 eye{0.0, 0.0, 5.0};
  constexpr glm::vec3 center{0.0, 0.0, 0.0};
  constexpr glm::vec3 Y_up{0.0, 1.0, 0.0}; // Camera orientation

  defaultView = glm::lookAt(eye, center, Y_up);
  return defaultView;
}

void update(const Camera& c, double dT) {
  delta = dT;
}

constexpr glm::vec3 defaultCameraPosition{0.0, 0.0, 5.0};
glm::vec3 eye = defaultCameraPosition;

glm::mat4x4 moveAround(const Camera& c, Camera::direction dir) {
  constexpr glm::vec3 Y_up{0.0, 1.0, 0.0}; // Camera orientation

  float speed = delta * 2.0;
  switch(dir) {
    using enum Camera::direction;

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
  return glm::lookAt(eye, glm::vec3(xy(eye), eye.z - 5.0f), Y_up);
}
