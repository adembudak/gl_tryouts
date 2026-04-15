#include "App.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imfilebrowser.h>

#include <vector>
#include <utility>
#include <ranges>
#include <print>

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

  pbr.baseColorLocation = glGetUniformLocation(programID, "pbr.baseColor");
  pbr.roughnessLocation = glGetUniformLocation(programID, "pbr.roughness");
  pbr.metallicLocation = glGetUniformLocation(programID, "pbr.metallic");

  pbr.baseColorTextureLocation.isDefined = glGetUniformLocation(programID, "baseColorTexture.isDefined");
  pbr.baseColorTextureLocation.sampler = glGetUniformLocation(programID, "baseColorTexture.sampler");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  p_fileDialog = new ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter};

  tn::PerspectiveCamera p;
  p.aspectRatio = 1.5;
  p.yfov = 0.660593;
  p.zfar = 100.0;
  p.znear = 0.01;
  defaultCamera = std::move(p);
  defaultCamera.name = "Default";

  constexpr glm::vec3 eye{0.0, 0.0, +5.0};      //   +y
  constexpr glm::vec3 center{0.0, 0.0, 0.0};    //   |
  constexpr glm::vec3 Y_up{0.0, +1.0, 0.0};     //   o-- +x
                                                //  /
  defaultView = glm::lookAt(eye, center, Y_up); // +z

  T t;
  t.view = &defaultView;
  t.perspective = defaultCamera.projectionMatrix();

  this->active_camera = "Default";
  cameras[active_camera] = t;

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
    putMenuBar();
  }

  ImGui::Begin("Scene");

  if(ImGui::BeginCombo("Cameras", active_camera.c_str())) {
    for(const auto& cam : cameraNames) {
      bool is_selected = (active_camera == cam);
      if(ImGui::Selectable(cam.c_str(), is_selected))
        active_camera = cam;

      if(is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::End();

  p_fileDialog->Display();

  if(p_fileDialog->HasSelected()) {
    std::println("Selected filename {}", p_fileDialog->GetSelected().string());
    is_scene_loaded = my_scene.load(p_fileDialog->GetSelected());
    this->loadSceneCameras();
    p_fileDialog->ClearSelected();
  }

  if(imgui_demo_window_visible) {
    ImGui::ShowDemoWindow(&imgui_demo_window_visible);
  }

  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  constexpr GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearBufferfv(GL_COLOR, 0, black);

  if(is_scene_loaded) {
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(cameras[active_camera].perspective));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(*cameras[active_camera].view));

    for(const auto& [_, node_buffer] : my_scene.getBuffers()) {
      if(node_buffer.type != node_t::type_t::mesh)
        continue;

      glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(node_buffer.transformMatrix()));

      glBindVertexArray(node_buffer.mesh_buffer.vertexArrayID);

      if(node_buffer.mesh_buffer.material.doubleSided) {
        glDisable(GL_CULL_FACE);
      } else {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
      }

      if(node_buffer.mesh_buffer.material.pbr.baseColorTexture.textureID != -1) {
        glUniform1i(pbr.baseColorTextureLocation.isDefined, true);
        glBindTextureUnit(0, node_buffer.mesh_buffer.material.pbr.baseColorTexture.textureID);
      } else {
        glUniform1i(pbr.baseColorTextureLocation.isDefined, false);
      }

      glUniform4fv(pbr.baseColorLocation, 1, std::data(node_buffer.mesh_buffer.material.pbr.baseColorFactor));
      glUniform1f(pbr.roughnessLocation, node_buffer.mesh_buffer.material.pbr.roughnessFactor);
      glUniform1f(pbr.metallicLocation, node_buffer.mesh_buffer.material.pbr.metallicFactor);

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
  my_scene.unload();
  shaderLoader.unload();

  delete p_fileDialog;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

void App::putMenuBar() {
  if(ImGui::BeginMenu("File")) {
    if(ImGui::MenuItem("Open scene")) {
      if(is_scene_loaded) { // if it's already open, close first
        is_scene_loaded = false;
        my_scene.unload();
      }
      p_fileDialog->Open();
    }

    if(ImGui::MenuItem("Close scene")) {
      is_scene_loaded = false;
      my_scene.unload();
    }

    ImGui::MenuItem("Dear ImGui demo", nullptr, &imgui_demo_window_visible);

    if(ImGui::MenuItem("Close")) {
      running = false;
    }

    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();
}

void App::loadSceneCameras() {
  for(auto& [id, node] : my_scene.getBuffers()) {
    if(node.camera.has_value()) {
      T t;
      t.view = &node.transformMatrix_;
      t.perspective = node.camera->projectionMatrix();
      cameras[node.camera->name] = std::move(t);
    }
  }

  if(!cameras.empty())
    cameraNames = cameras | std::ranges::views::keys | std::ranges::to<std::vector<std::string>>();
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
