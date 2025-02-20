#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find.hpp>
#include <mpark/patterns/match.hpp>

#include <iostream>
#include <sstream> // for std::ostringstream
#include <utility> // for std::unreacheble
#include <vector>
#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <cassert>
#include <numbers>

namespace util {

struct shaderLoader {
  std::vector<GLuint> shaderIDs;
  GLuint programID;

  shaderLoader& load(const std::vector<std::filesystem::path>& shaderFiles);
  shaderLoader& compile();
  shaderLoader& attach();
  shaderLoader& link();

  void emitProgramBinary() const;

  GLuint getProgramID() const {
    return programID;
  }

private:
  GLenum identifyShaderType(const std::filesystem::path shaderFile) const;
  std::string getShaderFileSource(const std::filesystem::path shaderFile) const;
};

shaderLoader& shaderLoader::load(const std::vector<std::filesystem::path>& shaderFiles) {
  for(auto shaderFile : shaderFiles) {
    std::string shaderSource = this->getShaderFileSource(shaderFile);
    auto data = std::data(shaderSource);
    auto size = std::size(shaderSource);

    GLenum type = this->identifyShaderType(shaderFile);

    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &data, nullptr);
    shaderIDs.push_back(shaderID);
  }

  return *this;
}

shaderLoader& shaderLoader::compile() {
  for(GLuint shaderID : shaderIDs)
    glCompileShader(shaderID);

  return *this;
}

shaderLoader& shaderLoader::attach() {
  programID = glCreateProgram();

  for(GLuint shaderID : shaderIDs)
    glAttachShader(programID, shaderID);

  return *this;
}

shaderLoader& shaderLoader::link() {
  glLinkProgram(programID);

  return *this;
}

void shaderLoader::emitProgramBinary() const {
  if(glIsProgram(programID)) {
    GLint binarySize;
    glGetProgramiv(programID, GL_PROGRAM_BINARY_LENGTH, &binarySize);

    std::vector<char> programBinary(binarySize, '\0');

    GLenum binaryFormat = GL_NONE;
    glGetProgramBinary(programID, binarySize, nullptr, &binaryFormat, std::data(programBinary));

    std::ofstream fout{"programBinary.bin", std::ios::binary};
    fout.write(std::data(programBinary), std::size(programBinary));
  }
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

std::string shaderLoader::getShaderFileSource(const std::filesystem::path shaderFile) const {
  std::ifstream fin{shaderFile};
  fin.unsetf(std::ifstream::skipws);

  return ranges::istream<char>(fin) | ranges::to<std::string>;
}

bool isExtensionAvailable(const std::string& ext) {
  GLint numExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

  // clang-format off
  auto
  features = ranges::views::iota(0, numExtensions)
           | ranges::views::transform([](int i) -> std::string { return reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)); })
           | ranges::to<std::vector<std::string>>;
  return ranges::find(features, ext) != std::end(features);
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

glm::vec3 eye(0.0, 0.0, 2.5);
glm::vec3 center(0.0, 0.0, 0.0);
glm::vec3 up(0.0, 1.0, 0.0);
glm::mat4x4 view = glm::lookAt(eye, center, up);

constexpr auto pi = std::numbers::pi_v<float>;

float field_of_view = pi / 2.0f;
float aspectRatio = 1.333f;
float zNear = 0.1f;
float zFar = 1000.0f;

glm::mat4x4 projection = glm::perspective(field_of_view, aspectRatio, zNear, zFar);
glm::mat4x4 transform = glm::mat4(1.0);

struct {
  std::array<GLenum, 3> mode = {GL_POINT, GL_LINE, GL_FILL};
  std::uint8_t i = 0;

  void operator++(int) {
    i = ++i % std::size(mode);
    std::cout << int(i);
    glPolygonMode(GL_FRONT_AND_BACK, mode[i]);
  }
} mesh_struct;

float rotateX = 0;
float rotateY = 0;
float rotateZ = 0;
const float rotateAmount = pi / 180.0;

void keyInput_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;

    case GLFW_KEY_W:      transform = glm::translate(transform, {0.0, 0.1, 0.0}); break;
    case GLFW_KEY_D:      transform = glm::translate(transform, {0.1, 0.0, 0.0}); break;
    case GLFW_KEY_S:      transform = glm::translate(transform, {0.0, -0.1, 0.0}); break;
    case GLFW_KEY_A:      transform = glm::translate(transform, {-0.1, 0.0, 0.0}); break;

    case GLFW_KEY_K:
      if(mods & GLFW_MOD_SHIFT)
        transform = glm::scale(transform, {1.1, 1.1, 1.1});
      else
        transform = glm::scale(transform, {0.9, 0.9, 0.9});
      break;

    case GLFW_KEY_X:
      if(mods & GLFW_MOD_SHIFT)
        transform = glm::rotate(transform, rotateX -= rotateAmount, {0.1, 0.0, 0.0});
      else
        transform = glm::rotate(transform, rotateX += rotateAmount, {0.1, 0.0, 0.0});
      break;
    case GLFW_KEY_Y: transform = glm::rotate(transform, rotateY += rotateAmount, {0.0, 1.0, 0.0}); break;
    case GLFW_KEY_Z: transform = glm::rotate(transform, rotateZ += rotateAmount, {0.0, 0.0, 1.0}); break;

    case GLFW_KEY_M: mesh_struct++; break;

    default:         break;
    }

    break;

  case GLFW_RELEASE: break;

  case GLFW_REPEAT:  break;

  default:           break;
  }
}

void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  switch(int(yoffset)) {
  case 1:  eye.y += 1.0; break;
  case -1: eye.y -= 1.0; break;
  default: break;
  }

  view = glm::lookAt(eye, center, up);
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
  glfwSetKeyCallback(window, keyInput_callback);
  glfwSetScrollCallback(window, mouseScroll_callback);
  glfwSetErrorCallback(errorCallback);

  if(glewInit() != GLEW_OK)
    return 3;

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_BLEND);
  glDebugMessageCallback(MessageCallback, nullptr);

  GLuint programID = util::shaderLoader{}
                         .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"}) //
                         .compile()
                         .attach()
                         .link()
                         .getProgramID();

  glUseProgram(programID);
  GLint tranformMatrixLocation = glGetUniformLocation(programID, "transform");
  GLint viewMatrixLocation = glGetUniformLocation(programID, "view");
  GLint projectionMatrixLocation = glGetUniformLocation(programID, "projection");

  using Vertex = glm::vec3;
  const std::vector<Vertex> vertexData = {
      {-0.5f, -0.5f, -0.5f},
      {0.5f,  -0.5f, -0.5f},
      {0.5f,  0.5f,  -0.5f},
      {-0.5f, 0.5f,  -0.5f},
      {-0.5f, -0.5f, 0.5f },
      {0.5f,  -0.5f, 0.5f },
      {0.5f,  0.5f,  0.5f },
      {-0.5f, 0.5f,  0.5f }
  };

  const std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 0, 3, 7, 0, 7, 4,
                                       1, 2, 6, 1, 6, 5, 0, 1, 5, 0, 5, 4, 3, 2, 6, 3, 6, 7};

  GLuint vertexArrayObject;
  glCreateVertexArrays(1, &vertexArrayObject);
  glBindVertexArray(vertexArrayObject);

  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLuint elementBufferID;
  glCreateBuffers(1, &elementBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

  GLsizeiptr sizeOfIndices = std::size(indices) * sizeof(decltype(indices[0])); // in bytes
  glNamedBufferStorage(elementBufferID, sizeOfIndices, std::data(indices), GL_MAP_READ_BIT);

  GLsizeiptr sizeOfVertices = std::size(vertexData) * sizeof(Vertex);

  glNamedBufferStorage(arrayBufferID, sizeOfVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);

  glNamedBufferSubData(arrayBufferID, 0, sizeOfVertices, std::data(vertexData));
  glVertexAttribPointer(0, vertexData[0].length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDisable(GL_CULL_FACE);

  assert(glGetError() == GL_NO_ERROR);

  const GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  while(!glfwWindowShouldClose(window)) {
    glClearBufferfv(GL_COLOR, 0, backgroundColor);

    glUniformMatrix4fv(tranformMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(vertexArrayObject);

    glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
