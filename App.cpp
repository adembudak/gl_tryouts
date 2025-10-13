#include "App.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <utility>

void App::setConfigDefaults() {
  info.title = "something something";
  AppBase::setConfigDefaults();
}

void App::startup() {

  programID = shaderLoader
                  .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"}) //
                  .compile()
                  .attach()
                  .link()
                  .getProgramID();

  my_scene.setProgramID(programID);
  my_scene.load("models/Models/Triangle/glTF/Triangle.gltf");

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void App::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  camera.update(delta);

  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

  const std::vector<buffer_t>& buffers = my_scene.getBuffers();
  for(const buffer_t& buffer : buffers) {
    glBindVertexArray(buffer.vertexArrayID);
    glDrawElements(buffer.element.mode, buffer.element.count, buffer.element.componentType, nullptr);
  }
}

void App::shutdown() {
  shaderLoader.unload();
  my_scene.unload();

  glfwDestroyWindow(window);
  glfwTerminate();
}

void App::onKey(int key, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE:
      AppBase::running = false;
      break;

    default:             break;
    }

    break;

  case GLFW_RELEASE: break;

  case GLFW_REPEAT:
    switch(key) {
    case GLFW_KEY_W: camera.moveAround(Camera::direction::front); break;
    case GLFW_KEY_D: camera.moveAround(Camera::direction::right); break;
    case GLFW_KEY_S: camera.moveAround(Camera::direction::back); break;
    case GLFW_KEY_A: camera.moveAround(Camera::direction::left); break;
    }
    break;

  default: break;
  }
}

void App::onMouseWheel(int pos) {
  switch(pos) {
  case 1:  camera.eye.z -= 1.0; break;
  case -1: camera.eye.z += 1.0; break;
  default: break;
  }
}

void App::onMouseMove(int x, int y) {
  float x_ = float(x) - (info.windowWidth / 2.0f);
  float y_ = float(y) - (info.windowHeight / 2.0f);
  camera.lookAround(glm::radians(x_), glm::radians(y_), 0.0f);
}
