#include <gl/glut.h>

struct Warna
{
   float r;
   float g;
   float b;
};

Warna buatWarna(float r, float g, float b)
{
   Warna w;
   w.r = r;
   w.g = g;
   w.b = b;
   return w;
}

float clamp01(float v)
{
   if (v < 0.0f) return 0.0f;
   if (v > 1.0f) return 1.0f;
   return v;
}

Warna ubahTerang(Warna c, float delta)
{
   Warna hasil;
   hasil.r = clamp01(c.r + delta);
   hasil.g = clamp01(c.g + delta);
   hasil.b = clamp01(c.b + delta);
   return hasil;
}

void gambarLatar(void)
{
   // Langit gradien
   glBegin(GL_QUADS);
      glColor3f(0.96f, 0.98f, 1.00f);
      glVertex2f(-2.0f, 1.2f);
      glVertex2f(2.0f, 1.2f);
      glColor3f(0.77f, 0.88f, 0.99f);
      glVertex2f(2.0f, -1.2f);
      glVertex2f(-2.0f, -1.2f);
   glEnd();

   // Lantai sederhana
   glBegin(GL_QUADS);
      glColor3f(0.64f, 0.76f, 0.92f);
      glVertex2f(-2.0f, -0.80f);
      glVertex2f(2.0f, -0.80f);
      glColor3f(0.50f, 0.64f, 0.84f);
      glVertex2f(2.0f, -1.2f);
      glVertex2f(-2.0f, -1.2f);
   glEnd();

   // Garis horizon
   glLineWidth(2.0f);
   glColor3f(0.34f, 0.52f, 0.76f);
   glBegin(GL_LINES);
      glVertex2f(-2.0f, -0.80f);
      glVertex2f(2.0f, -0.80f);
   glEnd();
}

void gambarKotak(float x, float y, float ukuran, Warna dasar)
{
   Warna warnaAtas = ubahTerang(dasar, 0.12f);
   Warna warnaBawah = ubahTerang(dasar, -0.10f);
   Warna warnaGaris = ubahTerang(dasar, -0.30f);

   // Isi kotak (gradasi atas-bawah)
   glBegin(GL_QUADS);
      glColor3f(warnaAtas.r, warnaAtas.g, warnaAtas.b);
      glVertex2f(x, y + ukuran);
      glVertex2f(x + ukuran, y + ukuran);
      glColor3f(warnaBawah.r, warnaBawah.g, warnaBawah.b);
      glVertex2f(x + ukuran, y);
      glVertex2f(x, y);
   glEnd();

   // Pita dekoratif pada bagian atas kotak
   Warna pita = ubahTerang(dasar, 0.22f);
   glColor3f(pita.r, pita.g, pita.b);
   glBegin(GL_QUADS);
      glVertex2f(x + ukuran * 0.08f, y + ukuran * 0.75f);
      glVertex2f(x + ukuran * 0.92f, y + ukuran * 0.75f);
      glVertex2f(x + ukuran * 0.92f, y + ukuran * 0.88f);
      glVertex2f(x + ukuran * 0.08f, y + ukuran * 0.88f);
   glEnd();

   // Aksen garis vertikal tipis
   glLineWidth(1.2f);
   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_LINES);
      glVertex2f(x + ukuran * 0.26f, y + ukuran * 0.12f);
      glVertex2f(x + ukuran * 0.26f, y + ukuran * 0.68f);
      glVertex2f(x + ukuran * 0.50f, y + ukuran * 0.12f);
      glVertex2f(x + ukuran * 0.50f, y + ukuran * 0.68f);
      glVertex2f(x + ukuran * 0.74f, y + ukuran * 0.12f);
      glVertex2f(x + ukuran * 0.74f, y + ukuran * 0.68f);
   glEnd();

   // Outline luar dan dalam
   glLineWidth(2.6f);
   glColor3f(warnaGaris.r, warnaGaris.g, warnaGaris.b);
   glBegin(GL_LINE_LOOP);
      glVertex2f(x, y);
      glVertex2f(x + ukuran, y);
      glVertex2f(x + ukuran, y + ukuran);
      glVertex2f(x, y + ukuran);
   glEnd();

   float pad = ukuran * 0.05f;
   glLineWidth(1.0f);
   glColor3f(0.97f, 0.99f, 1.0f);
   glBegin(GL_LINE_LOOP);
      glVertex2f(x + pad, y + pad);
      glVertex2f(x + ukuran - pad, y + pad);
      glVertex2f(x + ukuran - pad, y + ukuran - pad);
      glVertex2f(x + pad, y + ukuran - pad);
   glEnd();
}

void KotakBertingkat(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glLoadIdentity();
   gambarLatar();

   const float ukuran = 0.30f;

   // Tingkat atas: 1 kotak
   gambarKotak(-0.15f, -0.04f, ukuran, buatWarna(0.96f, 0.63f, 0.30f));

   // Tingkat tengah: 2 kotak
   gambarKotak(-0.34f, -0.40f, ukuran, buatWarna(0.35f, 0.73f, 0.97f));
   gambarKotak(0.04f, -0.40f, ukuran, buatWarna(0.42f, 0.78f, 0.52f));

   // Tingkat dasar: 3 kotak
   gambarKotak(-0.53f, -0.76f, ukuran, buatWarna(0.54f, 0.67f, 0.98f));
   gambarKotak(-0.15f, -0.76f, ukuran, buatWarna(0.95f, 0.45f, 0.62f));
   gambarKotak(0.23f, -0.76f, ukuran, buatWarna(0.60f, 0.56f, 0.98f));

   glFlush();
}

void AturProyeksi(int w, int h)
{
   if (h == 0) h = 1;

   float rasio = static_cast<float>(w) / static_cast<float>(h);
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   if (rasio >= 1.0f)
   {
      gluOrtho2D(-rasio, rasio, -1.0f, 1.0f);
   }
   else
   {
      gluOrtho2D(-1.0f, 1.0f, -1.0f / rasio, 1.0f / rasio);
   }

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(900, 650);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutCreateWindow("Kotak Bertingkat Kreatif");
   glutDisplayFunc(KotakBertingkat);
   glutReshapeFunc(AturProyeksi);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glutMainLoop();
   return 0;
}

