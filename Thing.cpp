#include "Thing.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>
#include <utility>

constexpr float rotateAmount = pi / 180.0;

void Thing::onKey(int key, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE:
      AppBase::running = false;
      break;
      // Translate model
    case GLFW_KEY_K: my_model.translate({0.0, 0.1, 0.0}); break;
    case GLFW_KEY_L: my_model.translate({0.1, 0.0, 0.0}); break;
    case GLFW_KEY_J: my_model.translate({0.0, -0.1, 0.0}); break;
    case GLFW_KEY_H: my_model.translate({-0.1, 0.0, 0.0}); break;

    case GLFW_KEY_M: // Scale model
      if(mods & GLFW_MOD_SHIFT)
        my_model.scale(glm::vec3{1.1, 1.1, 1.1});
      else
        my_model.scale(glm::vec3{0.9, 0.9, 0.9});
      break;

    case GLFW_KEY_X: // Rotate model
      if(mods & GLFW_MOD_SHIFT)
        my_model.rotate(-rotateAmount, {0.1, 0.0, 0.0});
      else
        my_model.rotate(rotateAmount, {0.1, 0.0, 0.0});
      break;

    case GLFW_KEY_Y:     my_model.rotate(rotateAmount, {0.0, 1.0, 0.0}); break;
    case GLFW_KEY_Z:     my_model.rotate(rotateAmount, {0.0, 0.0, 1.0}); break;

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
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  programID = shaderLoader
                  .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"}) //
                  .compile()
                  .attach()
                  .link()
                  .getProgramID();

  glUseProgram(programID);

  textureLoader.load("textures/Konyaalti.ktx");

  my_model.setProgramID(programID);
  my_model.transformMatrixLocation = glGetUniformLocation(programID, "transform");
  my_model.load("models/Models/Triangle/glTF/Triangle.gltf");

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void Thing::render(double currentTime) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Main");
  ImGui::Checkbox("Demo Window", &show_imgui_demo_window);
  ImGui::End();

  if(show_imgui_demo_window) {
    ImGui::ShowDemoWindow(&show_imgui_demo_window);
  }

  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  camera.update(delta);

  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  glUniformMatrix4fv(my_model.transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(my_model.transform));
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

  const std::vector<buffer_t>& buffers = my_model.getBuffers();
  for(const buffer_t& buffer : buffers) {
    glBindVertexArray(buffer.vertexArrayID);
    glDrawElements(buffer.element.mode, buffer.element.count, buffer.element.componentType, nullptr);
  }

  glBindTextureUnit(0, textureLoader.textureID);
  glDrawElements(GL_TRIANGLES, my_model.indiceSize, GL_UNSIGNED_INT, nullptr);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Thing::shutdown() {
  //  glDeleteVertexArrays(1, &my_model.vertexArrayID);
  glDeleteProgram(programID);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}
