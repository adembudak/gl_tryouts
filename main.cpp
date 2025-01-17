#include <GLFW/glfw3.h>

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
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);

  glfwMakeContextCurrent(window);

  glClearColor(0.43, 0.109, 0.203, 1.0); // Claret violet
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
