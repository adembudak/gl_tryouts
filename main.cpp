#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream> // for std::ostringstream
#include <utility> // for std::unreacheble
#include <vector>

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {

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

  sout << "Message: " << message;

  std::cout << sout.str();
}

const GLenum primitiveKinds[] = {GL_POINTS,
                                 GL_LINE_STRIP,
                                 GL_LINE_LOOP,
                                 GL_LINES,
                                 GL_LINE_STRIP_ADJACENCY,
                                 GL_LINES_ADJACENCY,
                                 GL_TRIANGLE_STRIP,
                                 GL_TRIANGLE_FAN,
                                 GL_TRIANGLES,
                                 GL_TRIANGLE_STRIP_ADJACENCY,
                                 GL_TRIANGLES_ADJACENCY};

const GLenum polygonModes[]{GL_POINT, GL_LINE, GL_FILL};

std::size_t primitiveKindSelector = 0;
std::size_t polygonModesSelector = 0;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    switch(key) {
    case GLFW_KEY_SPACE:  primitiveKindSelector = ++primitiveKindSelector % 11; break;
    case GLFW_KEY_ENTER:  polygonModesSelector = ++polygonModesSelector % 3; break;

    case GLFW_KEY_Q:      [[fallthrough]];
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }

    auto activePrimitiveKind = [](int ndx) {
      switch(ndx) {
      case 0:  return "GL_POINTS";
      case 1:  return "GL_LINE_STRIP";
      case 2:  return "GL_LINE_LOOP";
      case 3:  return "GL_LINES";
      case 4:  return "GL_LINE_STRIP_ADJACENCY";
      case 5:  return "GL_LINES_ADJACENCY";
      case 6:  return "GL_TRIANGLE_STRIP";
      case 7:  return "GL_TRIANGLE_FAN";
      case 8:  return "GL_TRIANGLES";
      case 9:  return "GL_TRIANGLE_STRIP_ADJACENCY";
      case 10: return "GL_TRIANGLES_ADJACENCY";
      default: std::unreachable(); break;
      }
    }(primitiveKindSelector);

    auto activePolygonMode = [](int ndx) {
      switch(ndx) {
      case 0:  return "GL_POINT";
      case 1:  return "GL_LINE";
      case 2:  return "GL_FILL";
      default: std::unreachable(); break;
      }
    }(polygonModesSelector);

    std::cout << activePrimitiveKind << ' ' << activePolygonMode << '\n';
  }
}

int main() {
  if(glfwInit() != GLFW_TRUE)
    return 1;

  GLFWwindow *window = glfwCreateWindow(640, 480, "GL", NULL, NULL);

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

  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

  const GLchar *const vertexShaderSource = R"(
  #version 460 core

  in vec2 vertexPosition;

  void main() {
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
  }
)";

  glShaderSource(vertexShaderID, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShaderID);

  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  const GLchar *const fragmentShaderSource = R"(
#version 460 core

out vec4 color;

void main() {
  color = vec4(0.945, 0.96, 0.964, 1.0); 
})";

  glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShaderID);

  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);

  glUseProgram(programID);

  // decagon
  std::vector<GLfloat> vertexData{
      0.0f,      0.0f,  1.0f, 0.0f,       0.809017f,  0.587785f,  0.309017f,  0.951057f, -0.309017f, 0.951057f, -0.809017f,
      0.587785f, -1.0f, 0.0f, -0.809017f, -0.587785f, -0.309017f, -0.951057f, 0.309017f, -0.951057f, 0.809017f, -0.587785f};

  GLuint vertexArrayID;
  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  GLuint arrayBufferID;
  glGenBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  auto data = std::data(vertexData);
  GLsizeiptr size = std::size(vertexData) * sizeof(decltype(vertexData[0]));

  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(0);

  glClearColor(0.43, 0.109, 0.203, 1.0); // Claret violet

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vertexArrayID);

    glPolygonMode(GL_FRONT_AND_BACK, polygonModes[polygonModesSelector]);
    glDrawArrays(primitiveKinds[primitiveKindSelector], 0, 25);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
