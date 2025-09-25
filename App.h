#pragma once

#include "AppBase.h"
#include "Model.h"
#include "ShaderLoader.h"
#include "Camera.h"

class App : public Application::AppBase {
private:
  GLuint programID;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;

  util::ShaderLoader shaderLoader;

  Model my_model;
  Camera camera;

  double lastTime = 0;

public:
  virtual void setConfigDefaults() override;
  virtual void startup() override;
  virtual void render(double currentTime) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;
  virtual void onMouseMove(int x, int y) override;
};
