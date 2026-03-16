#include <gl/glut.h>
#include <math.h>

void Lingkaran(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f(1.0f, 1.0f, 0.0f);

   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 360; i++) {
      float sudut = i * 3.14159f / 180.0f;
      glVertex2f(0.3f * cos(sudut), 0.3f * sin(sudut));
   }
   glEnd();

   glFlush();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(640, 480);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutCreateWindow("Lingkaran");
   glutDisplayFunc(Lingkaran);
   glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
   glutMainLoop();
   return 0;
}

