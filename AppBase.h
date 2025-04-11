#pragma once

#include <GL/glew.h>

#include <cstdint>
#include <string>

struct GLFWwindow;

namespace Application {

/*
 * void run() {
 *   glfwInit()
 *   this->init()
 *   glewInit();
 *
 *   this->startup();
 *
 *   while(running) {
 *     this->render();
 *     gflwPollEvents();
 *   }
 *
 *  this->shutdown();
 * }
 */

class AppBase {
private:
  static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
  bool isExtensionAvailable(const std::string& ext);

public:
  struct APPINFO {
    std::string title;
    int windowWidth;
    int windowHeight;
    int majorVersion;
    int minorVersion;
    int samples;
    union {
      struct {
        std::uint8_t fullscreen : 1;
        std::uint8_t vsync : 1;
        std::uint8_t cursor : 1;
        std::uint8_t stereo : 1;
        std::uint8_t debug : 1;
        std::uint8_t robust : 1;
        std::uint8_t : 2;
      };
      std::uint8_t all;
    } flags;
    static_assert(sizeof(decltype(flags)) == sizeof(std::uint8_t));
  };

protected:
  GLFWwindow* window;
  bool running;
  APPINFO info;

  static AppBase* app;

  static void error_callback(int error, const char* description);
  static void glfw_onResize(GLFWwindow* window, int w, int h);
  static void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void glfw_onMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void glfw_onMouseMove(GLFWwindow* window, double x, double y);
  static void glfw_onMouseWheel(GLFWwindow* window, double xoffset, double yoffset);

  void setVsync(bool enable);

public:
  virtual ~AppBase() = default;
  virtual void init();
  virtual void startup() = 0;
  virtual void render(double t) = 0;
  virtual void shutdown() = 0;

  void run(AppBase* the_app);

  virtual void onKey(int key, int action, int mods);
  virtual void onMouseButton(int button, int action);
  virtual void onMouseMove(int x, int y);
  virtual void onMouseWheel(int pos);
  virtual void onResize(int w, int h);
};

}
