#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace tinygltf {
struct PerspectiveCamera;
struct OrthographicCamera;
}

namespace tn = tinygltf;

struct Camera {
  Camera() = default;

  Camera(const tn::PerspectiveCamera& p);
  Camera(const tn::OrthographicCamera& o);

  glm::mat4x4 projectionMatrix() const;

  static const glm::mat4x4& defaultPerspectiveCamera();
  static const glm::mat4x4& defaultCameraPosition();

  glm::mat4x4 projection{};
};
