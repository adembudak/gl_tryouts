#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace tinygltf {
struct PerspectiveCamera;
struct OrthographicCamera;
}

namespace tn = tinygltf;

struct Camera {
  enum class direction { up, right, down, left, front, back } dir;

  Camera() = default;

  Camera(const tn::PerspectiveCamera& p);
  Camera(const tn::OrthographicCamera& o);

  glm::mat4x4 projectionMatrix() const;

  static glm::mat4x4 defaultPerspectiveCamera();
  static glm::mat4x4 defaultCameraPosition();

  glm::mat4x4 projection{};
};

void update(const Camera& c, double dT);
glm::mat4x4 moveAround(const Camera& c, Camera::direction dir);
