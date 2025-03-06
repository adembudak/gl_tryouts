#include "AppBase.h"
#include "Model.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <ktx.h>

#include <vector>
#include <array>
#include <filesystem>
#include <string>
#include <cassert>
#include <cstdint>

constexpr auto pi = glm::pi<float>();
constexpr float rotateAmount = pi / 180.0;

const std::vector<glm::vec3> vertexData = {
    {-0.5f, -0.5f, -0.5f},
    {0.5f,  -0.5f, -0.5f},
    {0.5f,  0.5f,  -0.5f},
    {-0.5f, 0.5f,  -0.5f},
    {-0.5f, -0.5f, 0.5f },
    {0.5f,  -0.5f, 0.5f },
    {0.5f,  0.5f,  0.5f },
    {-0.5f, 0.5f,  0.5f }
};

const std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 0, 3, 7, 0, 7, 4,
                                     1, 2, 6, 1, 6, 5, 0, 1, 5, 0, 5, 4, 3, 2, 6, 3, 6, 7};

const std::vector<glm::vec2> textureCoords = {
    // front
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // back
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // left
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // right
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // top
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // bottom
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
};

struct Camera {
  glm::vec3 eye{0.0, 0.0, 2.5};    // Gaze position
  glm::vec3 center{0.0, 0.0, 0.0}; // where to look point camera
  glm::vec3 up{0.0, 1.0, 0.0};     // camera orientation

  float fieldOfView = pi / 2.0f;
  float aspectRatio = 1.333f;
  float zNear = 0.1f;
  float zFar = 1000.0f;

  glm::mat4x4 yawPitchRoll = glm::mat4x4(1);
  glm::mat4x4 projection = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);

  void lookAround(float angleX, float angleY, float angleZ);

  glm::mat4x4 update(double dT);
};

glm::mat4x4 Camera::update(double dT) {
  return yawPitchRoll * glm::lookAt(eye, center, up);
}

void Camera::lookAround(float angleX, float angleY, float angleZ) {
  yawPitchRoll = glm::yawPitchRoll(angleX, angleY, 0.0f);
}

class Thing : public Application::AppBase {
private:
  GLuint programID;
  GLuint vertexArrayObject;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;

  util::TextureLoader textureLoader;
  util::ShaderLoader shaderLoader;

  Model cube;
  Camera camera;

public:
  virtual void init() override;
  virtual void startup() override;
  virtual void render(double t) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;
  virtual void onMouseMove(int x, int y) override;
};

void Thing::onKey(int key, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE: AppBase::running = false; break;

    case GLFW_KEY_W:      cube.translate({0.0, 0.1, 0.0}); break;
    case GLFW_KEY_D:      cube.translate({0.1, 0.0, 0.0}); break;
    case GLFW_KEY_S:      cube.translate({0.0, -0.1, 0.0}); break;
    case GLFW_KEY_A:      cube.translate({-0.1, 0.0, 0.0}); break;

    case GLFW_KEY_K:
      if(mods & GLFW_MOD_SHIFT)
        cube.scale(glm::vec3{1.1, 1.1, 1.1});
      else
        cube.scale(glm::vec3{0.9, 0.9, 0.9});
      break;

    case GLFW_KEY_X:
      if(mods & GLFW_MOD_SHIFT)
        cube.rotate(-rotateAmount, {0.1, 0.0, 0.0});
      else
        cube.rotate(rotateAmount, {0.1, 0.0, 0.0});
      break;

    case GLFW_KEY_Y: cube.rotate(rotateAmount, {0.0, 1.0, 0.0}); break;
    case GLFW_KEY_Z: cube.rotate(rotateAmount, {0.0, 0.0, 1.0}); break;

    case GLFW_KEY_M: Model::switchMeshMode(); break;

    default:         break;
    }

    break;

  case GLFW_RELEASE: break;

  case GLFW_REPEAT:  break;

  default:           break;
  }
}

void Thing::onMouseWheel(int pos) {
  switch(pos) {
  case 1:  camera.eye.y += 1.0; break;
  case -1: camera.eye.y -= 1.0; break;
  default: break;
  }
}

void Thing::onMouseMove(int x, int y) {
  camera.lookAround(glm::radians(double(x)), glm::radians(double(y)), 0.0);
}

void Thing::init() {
  info.title = "something something";
  AppBase::init();
}

void Thing::startup() {
  programID = shaderLoader
                  .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"}) //
                  .compile()
                  .attach()
                  .link()
                  .getProgramID();

  glUseProgram(programID);

  textureLoader.load("textures/Konyaalti.ktx");

  cube.transformMatrixLocation = glGetUniformLocation(programID, "transform");
  cube.load(vertexData, indices, textureCoords);

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void Thing::render(double t) {
  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  glUniformMatrix4fv(cube.transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube.transform));
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.update(0.0)));
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.projection));
  glBindVertexArray(cube.getVertexArrayID());

  glBindTextureUnit(0, textureLoader.textureID);
  glDrawElements(GL_TRIANGLES, cube.indiceSize, GL_UNSIGNED_INT, nullptr);
}

void Thing::shutdown() {
  glDeleteVertexArrays(1, &cube.vertexArrayID);
  glDeleteProgram(programID);
}

int main() {
  Thing* an_app = new Thing;
  an_app->run(an_app);
  delete an_app;
  return 0;
}
