#pragma once

#include "AppBase.h"
#include "Model.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "Camera.h"

class Thing : public Application::AppBase {
private:
  GLuint programID;
  GLuint vertexArrayObject;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;

  util::TextureLoader textureLoader;
  util::ShaderLoader shaderLoader;

  Model cube;
  Camera camera;

  double lastTime = 0;

  bool show_imgui_demo_window = 1;

public:
  virtual void init() override;
  virtual void startup() override;
  virtual void render(double currentTime) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;
  virtual void onMouseMove(int x, int y) override;
};
