#include "Thing.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <utility>

constexpr float rotateAmount = pi / 180.0;

std::vector<glm::vec3> vertexData = {
    {-0.5f, -0.5f, -0.5f},
    {0.5f,  -0.5f, -0.5f},
    {0.5f,  0.5f,  -0.5f},
    {-0.5f, 0.5f,  -0.5f},
    {-0.5f, -0.5f, 0.5f },
    {0.5f,  -0.5f, 0.5f },
    {0.5f,  0.5f,  0.5f },
    {-0.5f, 0.5f,  0.5f }
};

std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 0, 3, 7, 0, 7, 4,
                               1, 2, 6, 1, 6, 5, 0, 1, 5, 0, 5, 4, 3, 2, 6, 3, 6, 7};

std::vector<glm::vec2> textureCoords = {
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

void Thing::onKey(int key, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE:
      AppBase::running = false;
      break;
      // Translate model
    case GLFW_KEY_K: cube.translate({0.0, 0.1, 0.0}); break;
    case GLFW_KEY_L: cube.translate({0.1, 0.0, 0.0}); break;
    case GLFW_KEY_J: cube.translate({0.0, -0.1, 0.0}); break;
    case GLFW_KEY_H: cube.translate({-0.1, 0.0, 0.0}); break;

    case GLFW_KEY_M: // Scale model
      if(mods & GLFW_MOD_SHIFT)
        cube.scale(glm::vec3{1.1, 1.1, 1.1});
      else
        cube.scale(glm::vec3{0.9, 0.9, 0.9});
      break;

    case GLFW_KEY_X: // Rotate model
      if(mods & GLFW_MOD_SHIFT)
        cube.rotate(-rotateAmount, {0.1, 0.0, 0.0});
      else
        cube.rotate(rotateAmount, {0.1, 0.0, 0.0});
      break;

    case GLFW_KEY_Y:     cube.rotate(rotateAmount, {0.0, 1.0, 0.0}); break;
    case GLFW_KEY_Z:     cube.rotate(rotateAmount, {0.0, 0.0, 1.0}); break;

    case GLFW_KEY_SPACE: Model::switchMeshMode(); break;

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

void Thing::onMouseWheel(int pos) {
  switch(pos) {
  case 1:  camera.eye.z -= 1.0; break;
  case -1: camera.eye.z += 1.0; break;
  default: break;
  }
}

void Thing::onMouseMove(int x, int y) {
  float x_ = float(x) - (info.windowWidth / 2.0f);
  float y_ = float(y) - (info.windowHeight / 2.0f);
  camera.lookAround(glm::radians(x_), glm::radians(y_), 0.0f);
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

  cube.setProgramID(programID);
  cube.transformMatrixLocation = glGetUniformLocation(programID, "transform");
  cube.load(vertexData, indices, textureCoords);

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void Thing::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  camera.update(delta);

  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  glUniformMatrix4fv(cube.transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube.transform));
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));
  glBindVertexArray(cube.getVertexArrayID());

  glBindTextureUnit(0, textureLoader.textureID);
  glDrawElements(GL_TRIANGLES, cube.indiceSize, GL_UNSIGNED_INT, nullptr);
}

void Thing::shutdown() {
  glDeleteVertexArrays(1, &cube.vertexArrayID);
  glDeleteProgram(programID);

  glfwDestroyWindow(window);
  glfwTerminate();
}
