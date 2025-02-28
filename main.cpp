#include "AppBase.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <mpark/patterns/match.hpp>

#include <ktx.h>

#include <vector>
#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdint>
#include <iterator>

constexpr auto pi = glm::pi<float>();

const std::vector<glm::vec3> vertexData = {
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

const std::vector<glm::vec2> textureCoords = {
    // front
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // back
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // left
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // right
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // top
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},

    // bottom
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
};

namespace util {

struct textureLoader {
  GLuint textureID;
  GLenum target;

  bool load(const std::filesystem::path& textureFile);

  GLuint getTextureID() const {
    return textureID;
  }
};

bool textureLoader::load(const std::filesystem::path& textureFile) {
  assert(std::filesystem::exists(textureFile));

  ktxTexture* kTexture;
  KTX_error_code ret = ktxTexture_CreateFromNamedFile(textureFile.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);

  glGenTextures(1, &textureID);
  ret = ktxTexture_GLUpload(kTexture, &textureID, &target, nullptr);

  return true;
}

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

}

struct Camera {
  glm::vec3 eye{0.0, 0.0, 2.5};    // Gaze direction
  glm::vec3 center{0.0, 0.0, 0.0}; // where to look point camera
  glm::vec3 up{0.0, 1.0, 0.0};     // camera orientation

  float field_of_view = pi / 2.0f;
  float aspectRatio = 1.333f;
  float zNear = 0.1f;
  float zFar = 1000.0f;

  glm::mat4x4 projection = glm::perspective(field_of_view, aspectRatio, zNear, zFar);

public:
  glm::mat4x4 update(double t) {
    return glm::lookAt(eye, center, up);
  }
};

struct Model {
  static constexpr float rotateAmount = pi / 180.0;

  std::vector<glm::vec3> vertexPositions;
  std::vector<GLuint> indices;
  std::vector<glm::vec2> texturePositions;
  std::vector<glm::vec3> colors;
  glm::mat4x4 transform = glm::mat4(1.0);

  GLuint transformMatrixLocation;

  GLuint vertexArrayID;

  float rotateX = 0;
  float rotateY = 0;
  float rotateZ = 0;

  // void load(std::filesystem::path& modelFile);
  Model& load();
  GLuint getVertexArrayID() const {
    return vertexArrayID;
  }

private:
  Model& loadVertexPositions();
  Model& loadTexturePositions();
  Model& loadIndices();
};

Model& Model::load() {

  glCreateVertexArrays(1, &vertexArrayID);
  glBindVertexArray(vertexArrayID);

  loadVertexPositions();
  loadTexturePositions();
  loadIndices();

  return *this;
}

Model& Model::loadVertexPositions() {
  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLsizeiptr sizeOfVertex = sizeof(decltype(vertexPositions[0]));
  GLsizeiptr sizeOfVertices = std::size(vertexPositions) * sizeOfVertex;
  glNamedBufferStorage(arrayBufferID, sizeOfVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(arrayBufferID, 0, sizeOfVertices, std::data(vertexPositions));

  glVertexAttribPointer(0, vertexPositions[0].length(), GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
  glEnableVertexAttribArray(0);

  return *this;
}

Model& Model::loadTexturePositions() {
  GLuint arrayBufferID;
  glCreateBuffers(1, &arrayBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

  GLsizeiptr sizeOfTexelElement = sizeof(decltype(textureCoords[0]));
  GLsizeiptr sizeOfTexels = std::size(textureCoords) * sizeOfTexelElement;
  glNamedBufferStorage(arrayBufferID, sizeOfTexels, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(arrayBufferID, 0, sizeOfTexels, std::data(textureCoords));

  glVertexAttribPointer(1, textureCoords[0].length(), GL_FLOAT, GL_FALSE, sizeOfTexelElement, nullptr);
  glEnableVertexAttribArray(1);

  return *this;
}

Model& Model::loadIndices() {
  GLuint elementBufferID;
  glCreateBuffers(1, &elementBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

  GLsizeiptr sizeOfIndex = sizeof(decltype(indices[0]));
  GLsizeiptr sizeOfIndices = std::size(indices) * sizeOfIndex; // in bytes
  glNamedBufferStorage(elementBufferID, sizeOfIndices, std::data(indices), GL_MAP_READ_BIT);

  return *this;
}

struct {
  std::array<GLenum, 3> mode = {GL_POINT, GL_LINE, GL_FILL};
  std::uint8_t i = 0;

  void operator++(int) {
    i = ++i % std::size(mode);
    glPolygonMode(GL_FRONT_AND_BACK, mode[i]);
  }
} mesh_struct;

class Thing : public Application::AppBase {
private:
  GLuint programID;
  GLuint vertexArrayObject;

  GLuint viewMatrixLocation;
  GLuint projectionMatrixLocation;

  Model cube;
  Camera camera;

public:
  virtual void init() override;
  virtual void startup() override;
  virtual void render(double t) override;
  virtual void shutdown() override;

  virtual void onKey(int key, int action, int mods) override;
  virtual void onMouseWheel(int pos) override;
};

void Thing::onKey(int key, int action, int mods) {

  switch(action) {
  case GLFW_PRESS:
    switch(key) {
    case GLFW_KEY_ESCAPE: AppBase::running = false; break;

    case GLFW_KEY_W:      cube.transform = glm::translate(cube.transform, {0.0, 0.1, 0.0}); break;
    case GLFW_KEY_D:      cube.transform = glm::translate(cube.transform, {0.1, 0.0, 0.0}); break;
    case GLFW_KEY_S:      cube.transform = glm::translate(cube.transform, {0.0, -0.1, 0.0}); break;
    case GLFW_KEY_A:      cube.transform = glm::translate(cube.transform, {-0.1, 0.0, 0.0}); break;

    case GLFW_KEY_K:
      if(mods & GLFW_MOD_SHIFT)
        cube.transform = glm::scale(cube.transform, {1.1, 1.1, 1.1});
      else
        cube.transform = glm::scale(cube.transform, {0.9, 0.9, 0.9});
      break;

    case GLFW_KEY_X:
      if(mods & GLFW_MOD_SHIFT)
        cube.transform = glm::rotate(cube.transform, cube.rotateX -= Model::rotateAmount, {0.1, 0.0, 0.0});
      else
        cube.transform = glm::rotate(cube.transform, cube.rotateX += Model::rotateAmount, {0.1, 0.0, 0.0});
      break;

    case GLFW_KEY_Y:
      cube.transform = glm::rotate(cube.transform, cube.rotateY += Model::rotateAmount, {0.0, 1.0, 0.0});
      break;
    case GLFW_KEY_Z:
      cube.transform = glm::rotate(cube.transform, cube.rotateZ += Model::rotateAmount, {0.0, 0.0, 1.0});
      break;

    case GLFW_KEY_M: mesh_struct++; break;

    default:         break;
    }

    break;

  case GLFW_RELEASE: break;

  case GLFW_REPEAT:  break;

  default:           break;
  }
}

void Thing::onMouseWheel(int pos) {
  switch(pos) {
  case 1:  camera.eye.y += 1.0; break;
  case -1: camera.eye.y -= 1.0; break;
  default: break;
  }
}

void Thing::init() {
  info.title = "something something";
  AppBase::init();
}

void Thing::startup() {
  programID = util::shaderLoader{}
                  .load({"shaders/vertexShader.vert", "shaders/fragmentShader.frag"}) //
                  .compile()
                  .attach()
                  .link()
                  .getProgramID();

  glUseProgram(programID);

  util::textureLoader{}.load("textures/Konyaalti.ktx");

  cube.vertexPositions = std::move(vertexData);
  cube.texturePositions = std::move(textureCoords);
  cube.indices = std::move(indices);
  cube.transformMatrixLocation = glGetUniformLocation(programID, "transform");
  cube.load();

  viewMatrixLocation = glGetUniformLocation(programID, "view");
  projectionMatrixLocation = glGetUniformLocation(programID, "projection");

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  assert(glGetError() == GL_NO_ERROR);
}

void Thing::render(double t) {
  constexpr GLfloat backgroundColor[] = {0.43, 0.109, 0.203, 1.0}; // Claret violet
  constexpr GLfloat clearDepth = 1.0;
  glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
  glClearBufferfv(GL_DEPTH, 0, &clearDepth);

  glUniformMatrix4fv(cube.transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube.transform));
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.update(0.0)));
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.projection));
  glBindVertexArray(cube.getVertexArrayID());

  glDrawElements(GL_TRIANGLES, std::size(cube.indices), GL_UNSIGNED_INT, nullptr);
}

void Thing::shutdown() {
  glDeleteVertexArrays(1, &cube.vertexArrayID);
  glDeleteProgram(programID);
}

int main() {
  Thing* an_app = new Thing;
  an_app->run(an_app);
  delete an_app;
  return 0;
}
