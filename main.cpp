#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <range/v3/all.hpp>
#include <mpark/patterns.hpp>

#include <iostream>
#include <sstream> // for std::ostringstream
#include <utility> // for std::unreacheble
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>

namespace util {

struct shaderLoader {
  std::vector<GLuint> shaderIDs;

  GLuint programID;

  shaderLoader& load(const std::vector<std::filesystem::path>& shaderFiles);
  shaderLoader& compile();
  shaderLoader& attach();
  shaderLoader& link();

  GLuint getProgramID() const {
    return programID;
  }

private:
  GLenum identifyShaderType(const std::filesystem::path shaderFile) const;
};

shaderLoader& shaderLoader::load(const std::vector<std::filesystem::path>& shaderFiles) {
  for(auto shaderFile : shaderFiles) {
    std::ifstream fin{shaderFile};
    fin.unsetf(std::ifstream::skipws);

    std::string shaderSource = ranges::istream<char>(fin) | ranges::to<std::string>;

    auto data = std::data(shaderSource);
    auto size = std::size(shaderSource);

    GLenum type = this->identifyShaderType(shaderFile);
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &data, nullptr);

    shaderIDs.push_back(id);
  }

  return *this;
}

shaderLoader& shaderLoader::compile() {
  for(auto id : shaderIDs)
    glCompileShader(id);

  return *this;
}

shaderLoader& shaderLoader::attach() {
  programID = glCreateProgram();

  for(auto shaderID : shaderIDs)
    glAttachShader(programID, shaderID);

  return *this;
}

shaderLoader& shaderLoader::link() {
  glLinkProgram(programID);

  return *this;
}

GLenum shaderLoader::identifyShaderType(const std::filesystem::path shaderFile) const {
  using namespace mpark::patterns;
  // Based on: https://github.com/KhronosGroup/glslang?tab=readme-ov-file#execution-of-standalone-wrapper
  // clang-format off
  return match(shaderFile.extension().string())(
      pattern(".vert") = [] { return GL_VERTEX_SHADER; },         
      pattern(".tesc") = [] { return GL_TESS_CONTROL_SHADER; },   
      pattern(".tese") = [] { return GL_TESS_EVALUATION_SHADER; },
      pattern(".geom") = [] { return GL_GEOMETRY_SHADER; },       
      pattern(".frag") = [] { return GL_FRAGMENT_SHADER; },       
      pattern(".comp") = [] { return GL_COMPUTE_SHADER; }         
  );
  // clang-format on
}

}

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
  glEnable(GL_BLEND);
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

  const std::vector<Color> colors = {
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
  const std::vector<Vertex> vertexData = {
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

  const std::vector<GLuint> indices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  GLuint vertexArrayID;
  glGenVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  GLuint arrayBufferID;
  glGenBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLuint elementBufferID;
  glGenBuffers(1, &elementBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

  GLsizeiptr sizeOfIndices = std::size(indices) * sizeof(decltype(indices[0])); // in bytes
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndices, std::data(indices), GL_STATIC_DRAW);

  GLsizeiptr sizeOfVertices = std::size(vertexData) * sizeof(Vertex);
  GLsizeiptr sizeOfColors = std::size(colors) * sizeof(Color);

  glBufferData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfColors, nullptr, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfVertices, std::data(vertexData));
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(0);

  glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices, sizeOfColors, std::data(colors));
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), nullptr);
  glEnableVertexAttribArray(1);

  glClearColor(0.43, 0.109, 0.203, 1.0); // Claret violet
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vertexArrayID);

    // glDrawArrays(GL_TRIANGLE_FAN, 0, 10);
    // glDrawElements(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, nullptr);
    // glDrawElementsBaseVertex(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, nullptr, 0);
    // glDrawElementsInstanced(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, nullptr, 1);
    // glDrawElementsInstancedBaseVertex(GL_TRIANGLE_FAN, 10, GL_UNSIGNED_INT, nullptr, 1, 0);
    glDrawRangeElements(GL_TRIANGLE_FAN, 0, std::size(indices), 10, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
