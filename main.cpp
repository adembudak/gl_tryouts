#include <GLFW/glfw3.h>

#include <iostream>

int main() {
  if (!glfwInit())
    return 1;

  GLFWwindow *window = glfwCreateWindow(640, 480, "GL", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return 2;
  }

  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);

  glfwMakeContextCurrent(window);

  const GLubyte *gl_version = glGetString(GL_VERSION);
  std::cout << gl_version << '\n';

  GLint version[2];
  glGetIntegerv(GL_MAJOR_VERSION, &version[0]);
  glGetIntegerv(GL_MINOR_VERSION, &version[1]);
  std::cout << version[0] << '.' << version[1] << '\n';

  GLint numExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
  std::cout << numExtensions << '\n';

  glClearColor(0.43, 0.109, 0.203, 1.0); // Claret violet
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
