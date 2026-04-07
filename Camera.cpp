#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <tiny_gltf.h>

Camera::Camera(const tn::PerspectiveCamera& p) {
  projection = glm::perspective(p.yfov, p.aspectRatio, p.znear, p.zfar);
}

Camera::Camera(const tn::OrthographicCamera& o) {
  projection = glm::ortho(-o.xmag, o.xmag, -o.ymag, o.ymag, o.znear, o.zfar);
}

const glm::mat4x4& Camera::projectionMatrix() const {
  return projection;
}
