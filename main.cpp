#include <GL/freeglut.h>
#include <GL/gl.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

void handleKeyboard(unsigned char key, int x, int y) {
  switch(key) {
  case 'q': glutDestroyWindow(glutGetWindow()); break;
  case 't': glutFullScreenToggle(); break;
  }
}

void handleMouse(int btn, int state, int x, int y) {
  std::printf("%s %d %d\n", ((state == GLUT_DOWN) ? "down " : "up   "), x, y);
}

void handleReshape(int new_pos_w, int new_pos_h) { //
  std::printf("Reshaped to: %d %d\n", new_pos_w, new_pos_h);
  glViewport(0, 0, new_pos_w, new_pos_h);
  glutPostRedisplay();
}

void handleRendering() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.f, 0.f, 0.f, 1.f);

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

  const int win_x = 256;
  const int win_y = 256;
  glutInitWindowSize(win_x, win_y);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  int window_ret = glutCreateWindow("gl");
  if(window_ret < 1) std::exit(EXIT_FAILURE);
  std::printf("%s\n", glGetString(GL_VERSION));

  glutKeyboardUpFunc(handleKeyboard);
  glutMouseFunc(handleMouse);
  glutReshapeFunc(handleReshape);
  glutDisplayFunc(handleRendering);

  glutMainLoop();
}
