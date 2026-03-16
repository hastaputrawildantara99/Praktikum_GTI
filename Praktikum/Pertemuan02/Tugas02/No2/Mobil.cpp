#define GLUT_DISABLE_ATEXIT_HACK
#include "GL/glut.h"
#include <math.h>

float posisiMobil = -1.25f;
float rotasiRoda = 0.0f;
float rotasiMatahari = 0.0f;

void GambarLingkaran(float radiusX, float radiusY, int segmen)
{
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= segmen; i++) {
            float sudut = (2.0f * 3.14159f * i) / segmen;
            glVertex2f(radiusX * cos(sudut), radiusY * sin(sudut));
        }
    glEnd();
}

void GambarLangit(void)
{
    glPushMatrix();
        glBegin(GL_QUADS);
            glColor3f(0.38f, 0.70f, 0.98f);
            glVertex2f(-1.0f, 0.15f);
            glVertex2f( 1.0f, 0.15f);
            glColor3f(0.75f, 0.90f, 1.0f);
            glVertex2f( 1.0f, 1.0f);
            glVertex2f(-1.0f, 1.0f);
        glEnd();
    glPopMatrix();
}

void GambarJalan(void)
{
    glPushMatrix();
    // Aspal
        glColor3f(0.18f, 0.18f, 0.20f);
        glBegin(GL_QUADS);
            glVertex2f(-1.0f, -1.0f);
            glVertex2f( 1.0f, -1.0f);
            glVertex2f( 1.0f, -0.15f);
            glVertex2f(-1.0f, -0.15f);
        glEnd();

        // Trotoar
        glColor3f(0.55f, 0.55f, 0.58f);
        glBegin(GL_QUADS);
            glVertex2f(-1.0f, -0.15f);
            glVertex2f( 1.0f, -0.15f);
            glVertex2f( 1.0f, -0.05f);
            glVertex2f(-1.0f, -0.05f);
        glEnd();

        // Garis putus-putus jalan
        glColor3f(1.0f, 0.95f, 0.30f);
        for (float x = -0.95f; x < 1.0f; x += 0.28f) {
            glBegin(GL_QUADS);
                glVertex2f(x,        -0.60f);
                glVertex2f(x + 0.16f, -0.60f);
                glVertex2f(x + 0.16f, -0.56f);
                glVertex2f(x,        -0.56f);
            glEnd();
        }
    glPopMatrix();
}

void GambarMatahari(void)
{
    glPushMatrix();
        glTranslatef(0.76f, 0.74f, 0.0f);
        glRotatef(rotasiMatahari, 0.0f, 0.0f, 1.0f);

        glColor3f(1.0f, 0.90f, 0.25f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            for (int i = 0; i < 12; i++) {
                float sudut = i * 30.0f * 3.14159f / 180.0f;
                glVertex2f(0.0f, 0.0f);
                glVertex2f(0.12f * cos(sudut), 0.12f * sin(sudut));
            }
        glEnd();
        glLineWidth(1.0f);

        glColor3f(1.0f, 0.96f, 0.35f);
        GambarLingkaran(0.07f, 0.07f, 48);
    glPopMatrix();
}

void GambarAwan(float x, float y, float skala)
{
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(skala, skala, 1.0f);
        glColor3f(1.0f, 1.0f, 1.0f);

        glPushMatrix();
            glTranslatef(-0.08f, 0.0f, 0.0f);
            GambarLingkaran(0.07f, 0.05f, 32);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, 0.03f, 0.0f);
            GambarLingkaran(0.09f, 0.06f, 32);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.08f, 0.0f, 0.0f);
            GambarLingkaran(0.07f, 0.05f, 32);
        glPopMatrix();
    glPopMatrix();
}

void GambarPohon(float x, float y, float skala)
{
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(skala, skala, 1.0f);

        // Batang
        glColor3f(0.45f, 0.24f, 0.05f);
        glBegin(GL_QUADS);
            glVertex2f(-0.03f, 0.0f);
            glVertex2f( 0.03f, 0.0f);
            glVertex2f( 0.03f, 0.22f);
            glVertex2f(-0.03f, 0.22f);
        glEnd();

        // Daun (3 lapis)
        glColor3f(0.13f, 0.60f, 0.15f);
        glPushMatrix();
            glTranslatef(0.0f, 0.18f, 0.0f);
            GambarLingkaran(0.10f, 0.07f, 36);
        glPopMatrix();

        glColor3f(0.18f, 0.68f, 0.20f);
        glPushMatrix();
            glTranslatef(-0.07f, 0.24f, 0.0f);
            GambarLingkaran(0.09f, 0.07f, 36);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.07f, 0.24f, 0.0f);
            GambarLingkaran(0.09f, 0.07f, 36);
        glPopMatrix();
    glPopMatrix();
}

void GambarLampuMerah(float x, float y)
{
    glPushMatrix();
        glTranslatef(x, y, 0.0f);

        // Tiang
        glColor3f(0.35f, 0.35f, 0.35f);
        glBegin(GL_QUADS);
            glVertex2f(-0.012f, 0.0f);
            glVertex2f( 0.012f, 0.0f);
            glVertex2f( 0.012f, 0.52f);
            glVertex2f(-0.012f, 0.52f);
        glEnd();

        // Rumah lampu
        glColor3f(0.18f, 0.18f, 0.18f);
        glBegin(GL_QUADS);
            glVertex2f(-0.05f, 0.38f);
            glVertex2f( 0.05f, 0.38f);
            glVertex2f( 0.05f, 0.54f);
            glVertex2f(-0.05f, 0.54f);
        glEnd();

        glPushMatrix();
            glTranslatef(0.0f, 0.50f, 0.0f);
            glColor3f(1.0f, 0.05f, 0.05f);
            GambarLingkaran(0.017f, 0.017f, 24);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, 0.46f, 0.0f);
            glColor3f(0.35f, 0.35f, 0.05f);
            GambarLingkaran(0.017f, 0.017f, 24);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, 0.42f, 0.0f);
            glColor3f(0.05f, 0.35f, 0.05f);
            GambarLingkaran(0.017f, 0.017f, 24);
        glPopMatrix();
    glPopMatrix();
}

void GambarRoda(float x, float y, float rotasi)
{
    glPushMatrix();
        glTranslatef(x, y, 0.0f);

        glColor3f(0.10f, 0.10f, 0.10f);
        GambarLingkaran(0.09f, 0.09f, 48);

        glColor3f(0.70f, 0.70f, 0.72f);
        GambarLingkaran(0.05f, 0.05f, 36);

        glRotatef(rotasi, 0.0f, 0.0f, 1.0f);
        glColor3f(0.55f, 0.55f, 0.58f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            for (int i = 0; i < 6; i++) {
                float sudut = i * 60.0f * 3.14159f / 180.0f;
                glVertex2f(0.0f, 0.0f);
                glVertex2f(0.05f * cos(sudut), 0.05f * sin(sudut));
            }
        glEnd();
        glLineWidth(1.0f);
    glPopMatrix();
}

void GambarMobilSedan(float x, float y, float rotasi)
{
    glPushMatrix();
        glTranslatef(x, y, 0.0f); // Translasi mobil

        // Bayangan mobil
        glColor3f(0.08f, 0.08f, 0.08f);
        glBegin(GL_QUADS);
            glVertex2f(-0.35f, -0.13f);
            glVertex2f( 0.35f, -0.13f);
            glVertex2f( 0.30f, -0.11f);
            glVertex2f(-0.30f, -0.11f);
        glEnd();

        // Bodi bawah
        glColor3f(0.86f, 0.05f, 0.05f);
        glBegin(GL_QUADS);
            glVertex2f(-0.38f, -0.05f);
            glVertex2f( 0.38f, -0.05f);
            glVertex2f( 0.38f,  0.12f);
            glVertex2f(-0.38f,  0.12f);
        glEnd();

        // Kabin atas
        glBegin(GL_POLYGON);
            glVertex2f(-0.30f, 0.12f);
            glVertex2f(-0.18f, 0.30f);
            glVertex2f( 0.16f, 0.30f);
            glVertex2f( 0.34f, 0.18f);
            glVertex2f( 0.38f, 0.12f);
        glEnd();

        // Kaca depan
        glColor3f(0.70f, 0.88f, 0.98f);
        glBegin(GL_POLYGON);
            glVertex2f(0.10f, 0.14f);
            glVertex2f(0.31f, 0.14f);
            glVertex2f(0.31f, 0.20f);
            glVertex2f(0.20f, 0.28f);
        glEnd();

        // Kaca belakang
        glBegin(GL_POLYGON);
            glVertex2f(-0.26f, 0.14f);
            glVertex2f(-0.04f, 0.14f);
            glVertex2f(-0.04f, 0.28f);
            glVertex2f(-0.18f, 0.28f);
        glEnd();

        // Lampu depan
        glColor3f(1.0f, 0.98f, 0.70f);
        glBegin(GL_QUADS);
            glVertex2f(0.34f, 0.02f);
            glVertex2f(0.38f, 0.02f);
            glVertex2f(0.38f, 0.08f);
            glVertex2f(0.34f, 0.08f);
        glEnd();

        // Lampu belakang
        glColor3f(0.90f, 0.10f, 0.10f);
        glBegin(GL_QUADS);
            glVertex2f(-0.38f, 0.02f);
            glVertex2f(-0.34f, 0.02f);
            glVertex2f(-0.34f, 0.08f);
            glVertex2f(-0.38f, 0.08f);
        glEnd();

        // Roda 
        GambarRoda(-0.23f, -0.10f, rotasi);
        GambarRoda( 0.23f, -0.10f, rotasi);
    glPopMatrix();
}

void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GambarLangit();
    GambarMatahari();
    GambarAwan(-0.65f, 0.72f, 1.0f);
    GambarAwan(-0.15f, 0.80f, 0.8f);
    GambarAwan( 0.25f, 0.68f, 0.9f);

    GambarJalan();
    GambarPohon(-0.86f, -0.05f, 1.0f);
    GambarPohon(-0.62f, -0.05f, 0.85f);
    GambarPohon( 0.74f, -0.05f, 0.95f);
    GambarLampuMerah(0.55f, -0.05f);

    GambarMobilSedan(posisiMobil, -0.32f, rotasiRoda);

    glutSwapBuffers();
}

void Update(int value)
{
    posisiMobil += 0.006f;
    if (posisiMobil > 1.35f) {
        posisiMobil = -1.35f;
    }

    rotasiRoda -= 6.0f;
    if (rotasiRoda < -360.0f) {
        rotasiRoda += 360.0f;
    }

    rotasiMatahari += 0.25f;
    if (rotasiMatahari > 360.0f) {
        rotasiMatahari -= 360.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, Update, 0);
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(900, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("Mobil Sedan 2D");
    glClearColor(0.60f, 0.84f, 1.0f, 1.0f);

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutTimerFunc(16, Update, 0);
    glutMainLoop();
    return 0;
}

