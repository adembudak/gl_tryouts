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
  bool show_demo_window = false;

  bool is_scene_loaded = false;
  ImGui::FileBrowser* p_fileDialog;

  Camera defaultCamera;
  glm::mat4x4 defaultView;
  GLuint programID;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;
  GLuint transformMatrixLocation;

  GLuint baseColorLocation;
  GLuint roughnessLocation;
  GLuint metallicLocation;

  struct {
    GLboolean hasValue;
    GLuint sampler;
  } baseColorTextureLocation;

  util::ShaderLoader shaderLoader;

  Scene my_scene;

  double lastTime = 0;
};
