#include <GL/glew.h>
#include <GLFW/glfw3.h>

// #include <glm/glm.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/istream.hpp>
#include <mpark/patterns/match.hpp>

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
  return match(shaderFile.extension())(
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

  if(message != nullptr)
    sout << "Message: " << message << '\n';

  std::cout << sout.str();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

void errorCallback(int error, const char* description) {
  // Based on: https://www.glfw.org/docs/3.0/group__errors.html
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
  glfwSetKeyCallback(window, keyCallback);
  glfwSetErrorCallback(errorCallback);

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
  glCreateVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLuint elementBufferID;
  glCreateBuffers(1, &elementBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

  GLsizeiptr sizeOfIndices = std::size(indices) * sizeof(decltype(indices[0])); // in bytes
  glNamedBufferStorage(elementBufferID, sizeOfIndices, std::data(indices), GL_MAP_READ_BIT);

  GLsizeiptr sizeOfVertices = std::size(vertexData) * sizeof(Vertex);
  GLsizeiptr sizeOfColors = std::size(colors) * sizeof(Color);

  glNamedBufferStorage(arrayBufferID, sizeOfVertices + sizeOfColors, nullptr, GL_DYNAMIC_STORAGE_BIT);

  glNamedBufferSubData(arrayBufferID, 0, sizeOfVertices, std::data(vertexData));
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(0);

  glNamedBufferSubData(arrayBufferID, sizeOfVertices, sizeOfColors, std::data(colors));
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), nullptr);
  glEnableVertexAttribArray(1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  while(!glfwWindowShouldClose(window)) {
    glClearBufferfv(GL_COLOR, 0, backgroundColor);

    glBindVertexArray(vertexArrayID);

    glDrawRangeElements(GL_TRIANGLE_FAN, 0, std::size(indices), 10, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
