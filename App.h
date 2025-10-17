#pragma once

#include "AppBase.h"
#include "Scene.h"
#include "ShaderLoader.h"
#include "Camera.h"

struct App : public Application::AppBase {
  virtual void setConfigDefaults() override;
  virtual void startup() override;
  virtual void render(double currentTime) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;
  virtual void onMouseMove(int x, int y) override;

private:
  GLuint programID;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;
  GLuint transformMatrixLocation;

  util::ShaderLoader shaderLoader;

  Scene my_scene;
  Camera camera;

  double lastTime = 0;
};
