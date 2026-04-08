#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <tiny_gltf.h>

Camera::Camera(const tn::PerspectiveCamera& p) {
  projection = glm::perspective(p.yfov, p.aspectRatio, p.znear, p.zfar);
}

Camera::Camera(const tn::OrthographicCamera& o) {
  projection = glm::ortho(-o.xmag, o.xmag, -o.ymag, o.ymag, o.znear, o.zfar);
}

Camera::Camera(const Camera& cam) {
  projection = cam.projection;
  name = cam.name;
}

Camera& Camera::operator=(const Camera& cam) {
  projection = cam.projection;
  name = cam.name;

  return *this;
}

Camera::Camera(Camera&& cam) {
  projection = std::move(cam.projection);
  name = std::move(cam.name);
}

Camera& Camera::operator=(Camera&& cam) {
  projection = std::move(cam.projection);
  name = std::move(cam.name);

  return *this;
}

const glm::mat4x4& Camera::projectionMatrix() const {
  return projection;
}
