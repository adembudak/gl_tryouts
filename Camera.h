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

  const float* projectionMatrix() const;

  static const float* defaultPerspectiveCamera();
  static const float* defaultCameraPosition();


  glm::mat4x4 projection{};
};

void update(const Camera& c, double dT);
const float* moveAround(const Camera& c, Camera::direction dir);
