#include <iostream>
#include <GL/freeglut.h>

static void RenderSceneCB() {
    static GLclampf c = 0.0f;
    glClearColor(c, c, c, c);

    std::cout << "C: " << c << std::endl;

    c += 1.0f / 256.0f;

    if (c >= 1.0f) {
        c = 0.0f;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glutPostRedisplay();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    std::cout << "Hello, OpenGL!" << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    int width = 1920;
    int height = 1080;
    glutInitWindowSize(width, height);

    int x = 200;
    int y = 100;
    glutInitWindowPosition(x, y);

    int window = glutCreateWindow("Algebraic Graph Theory");
    std::cout << "Window ID: " << window << std::endl;

    GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
    glClearColor(Red, Green, Blue, Alpha);

    glutDisplayFunc(RenderSceneCB);

    glutMainLoop();

    return 0;
}
