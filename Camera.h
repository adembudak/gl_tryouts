#pragma once

#include <glm/mat4x4.hpp>

#include <string>

namespace tinygltf {
struct PerspectiveCamera;
struct OrthographicCamera;
}

namespace tn = tinygltf;

struct Camera {
  Camera() = default;

  Camera(const tn::PerspectiveCamera& p);
  Camera(const tn::OrthographicCamera& o);

  const glm::mat4x4& projectionMatrix() const;

  glm::mat4x4 projection{};
  std::string name;
};
