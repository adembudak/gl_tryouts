#include "shaderLoader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream> // for std::ostringstream
#include <utility> // for std::unreacheble
#include <vector>

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
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

  sout << "Message: " << message << '\n';

  std::cout << sout.str();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_Q:      [[fallthrough]];
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
    break;

  case GLFW_RELEASE:
    //
    break;
  case GLFW_REPEAT:
    //
    break;
  }
}

int main() {
  if(glfwInit() != GLFW_TRUE)
    return 1;

  GLFWwindow* window = glfwCreateWindow(640, 480, "GL", NULL, NULL);

  if(window == nullptr) {
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
  glfwSetKeyCallback(window, key_callback);

  if(glewInit() != GLEW_OK)
    return 3;

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  GLuint programID = util::shaderLoader{}
                         .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"})
                         .compile()
                         .attach()
                         .link()
                         .getProgramID();

  glUseProgram(programID);

  struct Vertex {
    GLfloat x, y;
  };

  struct Color {
    GLubyte r, g, b, a;
  };

  std::vector<Color> colors = {
      {12,  85,  184, 255},
      {215, 47,  102, 255},
      {89,  202, 53,  255},
      {203, 91,  18,  255},
      {76,  56,  215, 255},
      {122, 165, 9,   255},
      {231, 125, 76,  255},
      {180, 10,  160, 255},
      {3,   249, 134, 255},
      {53,  211, 238, 255}
  };

  // decagon
  std::vector<Vertex> vertexData{
      {1.0f,       0.0f      },
      {0.809017f,  0.587785f },
      {0.309017f,  0.951057f },
      {-0.309017f, 0.951057f },
      {-0.809017f, 0.587785f },
      {-1.0f,      0.0f      },
      {-0.809017f, -0.587785f},
      {-0.309017f, -0.951057f},
      {0.309017f,  -0.951057f},
      {0.809017f,  -0.587785f}
  };

  GLuint vertexArrayID;
  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  GLuint arrayBufferID;
  glGenBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLsizeiptr sizeOfVertices = std::size(vertexData) * sizeof(Vertex);
  GLsizeiptr sizeOfColors = std::size(colors) * sizeof(Color);

  glBufferData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfColors, nullptr, GL_STATIC_DRAW);

  auto vertexData_ = std::data(vertexData);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfVertices, vertexData_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(0);

  auto colorData_ = std::data(colors);
  glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices, sizeOfColors, colorData_);
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), nullptr);
  glEnableVertexAttribArray(1);

  glClearColor(0.43, 0.109, 0.203, 1.0); // Claret violet
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vertexArrayID);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 10);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
