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

  const char* glsl_version = "#version 460";
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void App::render(double currentTime) {
  const double delta = currentTime - lastTime;
  std::exchange(lastTime, currentTime);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  //  if(glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
  //   ImGui_ImplGlfw_Sleep(10);
  //          continue;
  //}

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if(ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    static ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
  }

  // 3. Show another simple window.
  if(show_another_window) {
    ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if(ImGui::Button("Close Me"))
      show_another_window = false;
    ImGui::End();
  }

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if(show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

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

    glUniform1i(baseColorTextureLocation.hasValue, false);
    if(node_buffer.mesh_buffer.material.baseColorTextureID != -1) {
      glUniform1i(baseColorTextureLocation.hasValue, true);
      glBindTextureUnit(0, node_buffer.mesh_buffer.material.baseColorTextureID);
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
