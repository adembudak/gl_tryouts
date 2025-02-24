#include "AppBase.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

namespace Application {
AppBase* AppBase::app = nullptr;

void AppBase::error_callback(int error, const char* description) {
  std::ostringstream sout;

  switch(error) {
  case GLFW_NOT_INITIALIZED:     sout << "GLFW has not been initialized."; break;
  case GLFW_NO_CURRENT_CONTEXT:  sout << "No context is current for this thread."; break;
  case GLFW_INVALID_ENUM:        sout << "One of the enum parameters for the function was given an invalid enum."; break;
  case GLFW_INVALID_VALUE:       sout << "One of the parameters for the function was given an invalid value."; break;
  case GLFW_OUT_OF_MEMORY:       sout << "A memory allocation failed."; break;
  case GLFW_API_UNAVAILABLE:     sout << "GLFW could not find support for the requested client API on the system."; break;
  case GLFW_VERSION_UNAVAILABLE: sout << "The requested client API version is not available."; break;
  case GLFW_PLATFORM_ERROR:      sout << "A platform-specific error occurred."; break;
  case GLFW_FORMAT_UNAVAILABLE:  sout << "The clipboard did not contain data in the requested format."; break;
  default:                       sout << "Unknown error code: " << error; break;
  }

  if(description != nullptr)
    sout << " Description: " << description;
  std::cout << sout.str() << '\n';
}

void AppBase::glfw_onResize(GLFWwindow* window, int w, int h) {
  app->onResize(w, h);
}

void AppBase::glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
  app->onKey(key, action);
}

void AppBase::glfw_onMouseButton(GLFWwindow* window, int button, int action, int mods) {
  app->onMouseButton(button, action);
}

void AppBase::glfw_onMouseMove(GLFWwindow* window, double x, double y) {
  app->onMouseMove(static_cast<int>(x), static_cast<int>(y));
}

void AppBase::glfw_onMouseWheel(GLFWwindow* window, double xoffset, double yoffset) {
  app->onMouseWheel(static_cast<int>(yoffset));
}

void AppBase::setVsync(bool enable) {
  info.flags.vsync = enable ? 1 : 0;
  glfwSwapInterval(info.flags.vsync);
}

void AppBase::init() {
  info.windowWidth = 800;
  info.windowHeight = 600;
  info.majorVersion = 4;
  info.minorVersion = 6;
  info.samples = 0;
  info.flags.all = 0;
  info.flags.cursor = 1;
#ifndef NDEBUG
  info.flags.debug = 1;
#endif
}

void AppBase::run(AppBase* the_app) {
  app = the_app;
  bool running = true;

  if(!glfwInit())
    return;

  init();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.majorVersion);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.minorVersion);

#ifndef NDEBUG
  if(info.flags.debug) {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  }
#endif

  if(info.flags.robust)
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, info.samples);
  glfwWindowHint(GLFW_STEREO, info.flags.stereo ? GLFW_TRUE : GLFW_FALSE);

  window = glfwCreateWindow(info.windowWidth, info.windowHeight, info.title.c_str(),
                            info.flags.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

  glfwMakeContextCurrent(window);
  glewInit();

  glfwSetErrorCallback(error_callback);
  glfwSetWindowSizeCallback(window, glfw_onResize);
  glfwSetKeyCallback(window, glfw_onKey);
  glfwSetMouseButtonCallback(window, glfw_onMouseButton);
  glfwSetCursorPosCallback(window, glfw_onMouseMove);
  glfwSetScrollCallback(window, glfw_onMouseWheel);

  if(!info.flags.cursor) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }

  info.flags.stereo = 0;

  if(info.flags.debug) {
    if(glfwExtensionSupported("GL_ARB_debug_output")) {
      glDebugMessageCallbackARB(MessageCallback, this);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    }
  }

  startup();

  while(running) {
    render(glfwGetTime());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  shutdown();
}

void AppBase::onKey(int key, int action) {}
void AppBase::onMouseButton(int button, int action) {}
void AppBase::onMouseMove(int x, int y) {}
void AppBase::onMouseWheel(int pos) {}
void AppBase::onResize(int w, int h) {
  info.windowWidth = w;
  info.windowHeight = h;
}

void GLAPIENTRY AppBase::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam) {

  std::ostringstream sout;
  sout << '\n';

  switch(sout << "Source: "; source) {
  case GL_DEBUG_SOURCE_API:             sout << "API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sout << "Window system"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER: sout << "Shader compiler"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:     sout << "Third party"; break;
  case GL_DEBUG_SOURCE_APPLICATION:     sout << "Application"; break;
  case GL_DEBUG_SOURCE_OTHER:           sout << "Other"; break;
  default:                              std::unreachable(); break;
  }
  sout << '\n';

  switch(sout << "Type: "; type) {
  case GL_DEBUG_TYPE_ERROR:               sout << "Error"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: sout << "Deprecated behavior"; break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  sout << "Undefined behavior"; break;
  case GL_DEBUG_TYPE_PORTABILITY:         sout << "Portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE:         sout << "Performance"; break;
  case GL_DEBUG_TYPE_MARKER:              sout << "Marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP:          sout << "Push group"; break;
  case GL_DEBUG_TYPE_POP_GROUP:           sout << "Pop group"; break;
  case GL_DEBUG_TYPE_OTHER:               sout << "Other"; break;
  default:                                std::unreachable(); break;
  }
  sout << '\n';

  switch(sout << "Severity: "; severity) {
  case GL_DEBUG_SEVERITY_LOW:          sout << "Low"; break;
  case GL_DEBUG_SEVERITY_MEDIUM:       sout << "Medium"; break;
  case GL_DEBUG_SEVERITY_HIGH:         sout << "High"; break;
  case GL_DEBUG_SEVERITY_NOTIFICATION: sout << "Notification"; break;
  default:                             std::unreachable(); break;
  }
  sout << '\n';

  if(message != nullptr)
    sout << "Message: " << message << '\n';

  std::cout << sout.str();
}

}
