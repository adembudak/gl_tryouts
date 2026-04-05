#include "App.h"

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imfilebrowser.h>

#include <vector>
#include <utility>
#include <iostream>

void App::setConfigDefaults() {
  info.title = "something something";
  AppBase::setConfigDefaults();
}

//////////////// ///////////// /////////////

void App::startup() {

  programID = shaderLoader
                  .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"}) //
                  .compile()
                  .attach()
                  .link()
                  .getProgramID();

  my_scene.setProgramID(programID);

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");
  transformMatrixLocation = glGetUniformLocation(programID, "transform");

  baseColorLocation = glGetUniformLocation(programID, "baseColor");
  roughnessLocation = glGetUniformLocation(programID, "roughness");
  metallicLocation = glGetUniformLocation(programID, "metallic");

  baseColorTextureLocation.hasValue = glGetUniformLocation(programID, "baseColorTexture.hasValue");
  baseColorTextureLocation.sampler = glGetUniformLocation(programID, "baseColorTexture.sampler");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  p_fileDialog = new ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter};

  assert(glGetError() == GL_NO_ERROR);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  ImGui::StyleColorsDark();

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
}

void App::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if(ImGui::BeginMainMenuBar()) {

    if(ImGui::BeginMenu("File")) {
      if(ImGui::MenuItem("Open scene")) {
        if(is_scene_loaded) {
          is_scene_loaded = false;
          my_scene.unload();
        }
        p_fileDialog->Open();
      }

      if(ImGui::MenuItem("Close scene")) {
        is_scene_loaded = false;
        my_scene.unload();
      }

      ImGui::MenuItem("Dear ImGui demo", nullptr, &show_demo_window);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  p_fileDialog->Display();

  if(p_fileDialog->HasSelected()) {
    is_scene_loaded = my_scene.load(p_fileDialog->GetSelected());
    std::cout << "Selected filename" << p_fileDialog->GetSelected().string() << std::endl;
    p_fileDialog->ClearSelected();
  }

  if(show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  constexpr GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearBufferfv(GL_COLOR, 0, black);

  if(is_scene_loaded) {

    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(Camera::defaultPerspectiveCamera()));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(Camera::defaultCameraPosition()));

    for(const auto& [_, node_buffer] : my_scene.getBuffers()) {
      glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(node_buffer.transformMatrix()));

      glBindVertexArray(node_buffer.mesh_buffer.vertexArrayID);

      if(node_buffer.mesh_buffer.material.doubleSided) {
        glDisable(GL_CULL_FACE);
      } else {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
      }

      if(node_buffer.mesh_buffer.material.baseColorTextureID != -1) {
        glUniform1i(baseColorTextureLocation.hasValue, true);
        glBindTextureUnit(0, node_buffer.mesh_buffer.material.baseColorTextureID);
      } else {
        glUniform1i(baseColorTextureLocation.hasValue, false);
      }

      glUniform4fv(baseColorLocation, 1, std::data(node_buffer.mesh_buffer.material.baseColorFactor));
      glUniform1f(roughnessLocation, node_buffer.mesh_buffer.material.roughnessFactor);
      glUniform1f(metallicLocation, node_buffer.mesh_buffer.material.metallicFactor);

      if(node_buffer.mesh_buffer.element.elementBufferID != -1)
        glDrawElements(node_buffer.mesh_buffer.element.mode, node_buffer.mesh_buffer.element.count, node_buffer.mesh_buffer.element.componentType, nullptr);
      else
        glDrawArrays(node_buffer.mesh_buffer.element.mode, 0, node_buffer.mesh_buffer.count);
    }

    my_scene.animate(currentTime);
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::shutdown() {
  shaderLoader.unload();
  my_scene.unload();

  delete p_fileDialog;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

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
