#include <GL/glew.h>

#include <GL/freeglut.h>

#include <cstdio>
#include <cstdlib>

void handleKeyboard(unsigned char key, int x, int y) {
  switch(key) {
  case 'q': glutDestroyWindow(glutGetWindow()); break;
  case 't': glutFullScreenToggle(); break;
  }
}

void handleMouse(int btn, int state, int x, int y) {
  auto showState = [=](const char *s) {
    switch(btn) {
    case GLUT_LEFT_BUTTON: printf("Left mouse button: %s %d %d\n", s, x, y); break;
    case GLUT_RIGHT_BUTTON: printf("Right mouse button: %s %d %d\n", s, x, y); break;
    default: break;
    }
  };

  switch(state) {
  case GLUT_DOWN: showState("Down"); break;
  case GLUT_UP: showState("Up"); break;
  default: break;
  }
}

void handleReshape(int new_pos_w, int new_pos_h) { //
  std::printf("Reshaped to: %d %d\n", new_pos_w, new_pos_h);
  glViewport(0, 0, new_pos_w, new_pos_h);
  glutPostRedisplay();
}

int frameCount = 0;

const GLchar *const VertexShaderSourceCode =
    R"(#version 400
     layout(location=0) in vec4 in_Position;
     layout(location=1) in vec4 in_Color;
     out vec4 ex_Color;

     void main(void) {
       gl_Position = in_Position;
       ex_Color = in_Color;
     })";

const GLchar *const FragmentShaderSourceCode =
    R"(#version 400
    in  vec4 ex_Color;
    out vec4 out_Color;

    void main(void) {
      out_Color = ex_Color;
    })";

void handleRendering() {
  ++frameCount;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glutSwapBuffers();
}

void timerFunction(int t) {
  if(0 != t) {
    char strBuf[1024]{};
    std::sprintf(strBuf, "%d Frames Per Second", frameCount * 4);
    glutSetWindowTitle(strBuf);
  }

  frameCount = 0;
  glutTimerFunc(250, timerFunction, 1);
}

void idleFunction() {
  glutPostRedisplay();
}

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLuint VaoId, VboId, ColorBufferId;

void createShaders() {
  VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(VertexShaderId, 1, &VertexShaderSourceCode, NULL);
  glCompileShader(VertexShaderId);

  FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(FragmentShaderId, 1, &FragmentShaderSourceCode, NULL);
  glCompileShader(FragmentShaderId);

  ProgramId = glCreateProgram();
  glAttachShader(ProgramId, VertexShaderId);
  glAttachShader(ProgramId, FragmentShaderId);
  glLinkProgram(ProgramId);
  glUseProgram(ProgramId);

  if(GLenum ErrorCheckValue = glGetError(); ErrorCheckValue != GL_NO_ERROR) {
    std::fprintf(stderr, "ERROR: Could not create the shaders: %s \n", gluErrorString(ErrorCheckValue));
    std::exit(EXIT_FAILURE);
  }
}

void destroyShaders() {
  glUseProgram(0);

  glDetachShader(ProgramId, VertexShaderId);
  glDetachShader(ProgramId, FragmentShaderId);

  glDeleteShader(FragmentShaderId);
  glDeleteShader(VertexShaderId);

  glDeleteProgram(ProgramId);

  if(GLenum ErrorCheckValue = glGetError(); ErrorCheckValue != GL_NO_ERROR) {
    std::fprintf(stderr, "ERROR: Could not destroy the shaders: %s \n", gluErrorString(ErrorCheckValue));
    std::exit(EXIT_FAILURE);
  }
}

void createVBO() {
  GLfloat Vertices[] = {
      -0.8f, -0.8f, 0.0f, 1.0f, //
      0.0f,  0.8f,  0.0f, 1.0f, //
      0.8f,  -0.8f, 0.0f, 1.0f  //
  };

  GLfloat Colors[] = {
      1.0f, 0.0f, 0.0f, 1.0f, //
      0.0f, 1.0f, 0.0f, 1.0f, //
      0.0f, 0.0f, 1.0f, 1.0f  //
  };

  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);

  glGenBuffers(1, &VboId);
  glBindBuffer(GL_ARRAY_BUFFER, VboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &ColorBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  if(GLenum ErrorCheckValue = glGetError(); ErrorCheckValue != GL_NO_ERROR) {
    std::fprintf(stderr, "ERROR: Could not create VBO: %s \n", gluErrorString(ErrorCheckValue));
    std::exit(EXIT_FAILURE);
  }
}

void destroyVBO() {
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &ColorBufferId);
  glDeleteBuffers(1, &VboId);

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VaoId);

  if(GLenum ErrorCheckValue = glGetError(); ErrorCheckValue != GL_NO_ERROR) {
    std::fprintf(stderr, "ERROR: Could not destroy VBO: %s \n", gluErrorString(ErrorCheckValue));
    std::exit(EXIT_FAILURE);
  }
}

void cleanUp() {
  destroyShaders();
  destroyVBO();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(3, 0);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  glutInitWindowSize(256, 256);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  int window_ret = glutCreateWindow("gl");
  if(window_ret < 1) std::exit(EXIT_FAILURE);

  if(glutDeviceGet(GLUT_HAS_KEYBOARD)) {
    glutKeyboardUpFunc(handleKeyboard);
  }

  if(glutDeviceGet(GLUT_HAS_MOUSE)) {
    glutMouseFunc(handleMouse);
  }

  glutReshapeFunc(handleReshape);
  glutDisplayFunc(handleRendering);
  glutIdleFunc(idleFunction);
  glutTimerFunc(0, timerFunction, 0);
  glutCloseFunc(cleanUp);

  glewExperimental = GL_TRUE;
  GLenum GlewInitResult = glewInit();
  if(GLEW_OK != GlewInitResult) std::exit(EXIT_FAILURE);

  createShaders();
  createVBO();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glutMainLoop();
}
