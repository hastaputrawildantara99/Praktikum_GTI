#include <gl/glut.h>

void titikTanpaGLPoint(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f(1.0f, 0.0f, 0.0f);

   const float x = 0.25f;
   const float y = 0.25f;
   const float ukuranPixel = 5.0f;

   int lebar = glutGet(GLUT_WINDOW_WIDTH);
   int tinggi = glutGet(GLUT_WINDOW_HEIGHT);
   if (lebar <= 0 || tinggi <= 0)
   {
      glFlush();
      return;
   }

   // Konversi ukuran pixel ke koordinat OpenGL (-1 s.d. 1).
   float dx = (ukuranPixel / static_cast<float>(lebar)) * 2.0f;
   float dy = (ukuranPixel / static_cast<float>(tinggi)) * 2.0f;
   float setengahDx = dx * 0.5f;
   float setengahDy = dy * 0.5f;

   glBegin(GL_QUADS);
      glVertex2f(x - setengahDx, y - setengahDy);
      glVertex2f(x + setengahDx, y - setengahDy);
      glVertex2f(x + setengahDx, y + setengahDy);
      glVertex2f(x - setengahDx, y + setengahDy);
   glEnd();

   glFlush();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(640, 480);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutCreateWindow("Titik Tanpa GL_POINT");
   glutDisplayFunc(titikTanpaGLPoint);
   glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
   glutMainLoop();
   return 0;
}

