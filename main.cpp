#include <GL/freeglut.h>
#include <GL/gl.h>

#include <iostream>

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  const int win_x = 1080 / 4;
  const int win_y = 720 / 4;
  glutInitWindowSize(win_x, win_y);

  glutCreateWindow("gl");

  glutKeyboardUpFunc([](unsigned char key, int x, int y) {
    switch(key) {
    case 'q': glutDestroyWindow(glutGetWindow()); break;
    case 't': glutFullScreenToggle(); break;
    }
  });

  glutMouseFunc([](int btn, int state, int x, int y) {
    std::cout << ((state == GLUT_DOWN) ? "down " : "up   ") << x << ' ' << y << '\n';
  });

  glutDisplayFunc([]() { //
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    glutSwapBuffers();
  });

  glutReshapeFunc([](int new_pos_x, int new_pos_y) { //
    std::cout << "Reshaped to: " << new_pos_x << ' ' << new_pos_y << '\n';
    glutPostRedisplay();
  });

  glutMainLoop();
}
