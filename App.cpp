#include "App.h"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

  my_scene.load("/home/adem/Github/gl_tryouts/models/Models/AnimatedTriangle/glTF-Embedded/AnimatedTriangle.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/SimpleTexture/glTF/SimpleTexture.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/Triangle/glTF/Triangle.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/Cameras/glTF/Cameras.gltf");
  // my_scene.load("/home/adem/Github/gl_tryouts/models/Models/ABeautifulGame/glTF/ABeautifulGame.gltf");

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

  assert(glGetError() == GL_NO_ERROR);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  // Setup Platform/Renderer backends
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

  ImGui::Begin("Hello, world!");                     // Create a window called "Hello, world!" and append into it.
  ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
  ImGui::End();

  if(show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  constexpr GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearBufferfv(GL_COLOR, 0, black);

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

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::shutdown() {
  shaderLoader.unload();
  my_scene.unload();

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
