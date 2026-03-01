#include "App.h"

#include <glm/gtc/type_ptr.hpp>
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
  my_scene.load("/home/adem/Github/gl_tryouts/models/Models/SimpleMaterial/glTF-Embedded/SimpleMaterial.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/Triangle/glTF/Triangle.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/Cameras/glTF/Cameras.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");
  transformMatrixLocation = glGetUniformLocation(programID, "transform");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void App::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  ////////////////  camera.update(delta);

  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  constexpr GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearBufferfv(GL_COLOR, 0, black);

  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(Camera::defaultPerspectiveCamera()));
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(Camera::defaultCameraPosition()));

  for(const node_t& node_buffer : my_scene.getBuffers()) {
    glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(node_buffer.transformMatrix()));

    glBindVertexArray(node_buffer.mesh_buffer.vertexArrayID);

    glUniform1d(glGetUniformLocation(programID, "roughness"), node_buffer.mesh_buffer.material.roughnessFactor);
    glUniform4dv(glGetUniformLocation(programID, "baseColor"), 1, std::data(node_buffer.mesh_buffer.material.baseColorFactor));
    glUniform1d(glGetUniformLocation(programID, "metallic"), node_buffer.mesh_buffer.material.metallicFactor);

    if(node_buffer.mesh_buffer.element.elementBufferID != -1)
      glDrawElements(node_buffer.mesh_buffer.element.mode, node_buffer.mesh_buffer.element.count, node_buffer.mesh_buffer.element.componentType, nullptr);
    else
      glDrawArrays(node_buffer.mesh_buffer.element.mode, 0, node_buffer.mesh_buffer.count);
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
    case GLFW_KEY_ESCAPE: AppBase::running = false; break;

    default:              break;
    }

    break;

  case GLFW_RELEASE: break;

  case GLFW_REPEAT:
    switch(key) {
    case GLFW_KEY_W: break;
    case GLFW_KEY_D: break;
    case GLFW_KEY_S: break;
    case GLFW_KEY_A: break;
    }
    break;

  default: break;
  }
}

void App::onMouseWheel(int pos) {
  switch(pos) {
  case 1:  break;
  case -1: break;
  default: break;
  }
}
