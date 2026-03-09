#include <gl/glut.h>

void GarisPanjang(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glBegin(GL_QUADS);

   glColor3f(1.0f, 1.0f, 1.0f);
   // Garis vertikal tipis, tapi panjang.
   glVertex3f(-0.01f, 0.80f, 0.0f);
   glVertex3f(0.01f, 0.80f, 0.0f);
   glVertex3f(0.01f, -0.80f, 0.0f);
   glVertex3f(-0.01f, -0.80f, 0.0f);

   glEnd();
   glFlush();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(640, 480);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutCreateWindow("Membuat Garis Panjang");
   glutDisplayFunc(GarisPanjang);
   glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
   glutMainLoop();
   return 0;
}

