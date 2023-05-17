#include <GL/freeglut.h>
#include <GL/gl.h>

#include <iostream>

void handleKeyboard(unsigned char key, int x, int y) {
  switch(key) {
  case 'q': glutDestroyWindow(glutGetWindow()); break;
  case 't': glutFullScreenToggle(); break;
  }
}

void handleMouse(int btn, int state, int x, int y) {
  std::cout << ((state == GLUT_DOWN) ? "down " : "up   ") << x << ' ' << y << '\n';
}

void handleReshape(int new_pos_w, int new_pos_h) { //
  std::cout << "Reshaped to: " << new_pos_w << ' ' << new_pos_h << '\n';
  glViewport(0, 0, new_pos_w, new_pos_h);
  glutPostRedisplay();
}

void handleRendering() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.f, 0.f, 0.f, 1.f);

  glutSwapBuffers();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  const int win_x = 256;
  const int win_y = 256;
  glutInitWindowSize(win_x, win_y);

  glutCreateWindow("gl");
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  glutKeyboardUpFunc(handleKeyboard);
  glutMouseFunc(handleMouse);
  glutReshapeFunc(handleReshape);
  glutDisplayFunc(handleRendering);

  glutMainLoop();
}
