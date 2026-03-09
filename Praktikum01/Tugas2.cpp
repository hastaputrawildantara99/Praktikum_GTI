#include <gl/glut.h>

void GambarPrimitiveGabungan(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glLineWidth(2.0f);

   // 1) GL_LINE_STRIP
   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_LINE_STRIP);
      glVertex2f(-0.90f, 0.75f);
      glVertex2f(-0.78f, 0.90f);
      glVertex2f(-0.66f, 0.75f);
      glVertex2f(-0.54f, 0.90f);
      glVertex2f(-0.42f, 0.75f);
   glEnd();

   // 2) GL_LINE_LOOP
   glColor3f(1.0f, 0.6f, 0.1f);
   glBegin(GL_LINE_LOOP);
      glVertex2f(0.45f, 0.75f);
      glVertex2f(0.55f, 0.90f);
      glVertex2f(0.72f, 0.90f);
      glVertex2f(0.82f, 0.75f);
      glVertex2f(0.72f, 0.60f);
      glVertex2f(0.55f, 0.60f);
   glEnd();

   // 3) GL_TRIANGLE_FAN
   glBegin(GL_TRIANGLE_FAN);
      glColor3f(0.9f, 0.1f, 0.2f);
      glVertex2f(-0.70f, 0.10f);  // Titik pusat
      glColor3f(1.0f, 0.8f, 0.2f);
      glVertex2f(-0.85f, 0.00f);
      glVertex2f(-0.78f, 0.20f);
      glVertex2f(-0.62f, 0.24f);
      glVertex2f(-0.50f, 0.10f);
      glVertex2f(-0.62f, -0.04f);
      glVertex2f(-0.78f, -0.02f);
      glVertex2f(-0.85f, 0.00f);  // Tutup kipas
   glEnd();

   // 4) GL_TRIANGLE_STRIP
   glBegin(GL_TRIANGLE_STRIP);
      glColor3f(0.2f, 0.9f, 0.4f);
      glVertex2f(-0.20f, 0.25f);
      glVertex2f(-0.20f, -0.05f);
      glVertex2f(-0.05f, 0.28f);
      glVertex2f(-0.05f, -0.08f);
      glVertex2f(0.10f, 0.22f);
      glVertex2f(0.10f, -0.02f);
      glVertex2f(0.25f, 0.18f);
      glVertex2f(0.25f, 0.02f);
   glEnd();

   // 5) GL_QUADS
   glBegin(GL_QUADS);
      glColor3f(0.2f, 0.7f, 1.0f);
      glVertex2f(-0.90f, -0.45f);
      glVertex2f(-0.55f, -0.45f);
      glVertex2f(-0.55f, -0.75f);
      glVertex2f(-0.90f, -0.75f);
   glEnd();

   // 6) GL_QUAD_STRIP
   glBegin(GL_QUAD_STRIP);
      glColor3f(0.9f, 0.3f, 1.0f);
      glVertex2f(0.35f, -0.75f); glVertex2f(0.35f, -0.45f);
      glVertex2f(0.50f, -0.78f); glVertex2f(0.50f, -0.40f);
      glVertex2f(0.65f, -0.75f); glVertex2f(0.65f, -0.45f);
      glVertex2f(0.80f, -0.70f); glVertex2f(0.80f, -0.50f);
   glEnd();

   glFlush();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(800, 600);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutCreateWindow("Demo Primitive OpenGL");
   glutDisplayFunc(GambarPrimitiveGabungan);
   glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
   glutMainLoop();
   return 0;
}

