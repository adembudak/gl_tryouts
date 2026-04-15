#pragma once

#include "AppBase.h"
#include "Scene.h"
#include "ShaderLoader.h"

namespace ImGui {
class FileBrowser;
}

struct App : public Application::AppBase {
  virtual void setConfigDefaults() override;
  virtual void startup() override;
  virtual void render(double currentTime) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;

private:
  void putMenuBar();

  void loadSceneCameras();
  struct T {
    glm::mat4x4* view;
    glm::mat4x4 perspective;
  };

  std::map<std::string, T> cameras;
  std::vector<std::string> cameraNames;
  std::string active_camera;

private:

  bool imgui_demo_window_visible = false;

  bool is_scene_loaded = false;
  ImGui::FileBrowser* p_fileDialog;

  Camera defaultCamera;
  glm::mat4x4 defaultView;

  GLuint programID;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;
  GLuint transformMatrixLocation;

  struct {
    GLuint baseColorLocation;
    GLuint roughnessLocation;
    GLuint metallicLocation;

    struct {
      GLboolean isDefined;
      GLuint sampler;
    } baseColorTextureLocation;

    struct {
      GLboolean isDefined;
      GLuint sampler;
    } metallicRoughnessTextureLocation;
  } pbr;

  util::ShaderLoader shaderLoader;

  Scene my_scene;

  double lastTime = 0;
};
