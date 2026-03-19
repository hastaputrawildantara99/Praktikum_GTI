#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef GLUT_RGB
#define GLUT_RGB    0
#endif
#ifndef GLUT_RGBA
#define GLUT_RGBA   GLUT_RGB
#endif
#ifndef GLUT_DOUBLE
#define GLUT_DOUBLE 2
#endif
#ifndef GLUT_DEPTH
#define GLUT_DEPTH  16
#endif

#ifndef APIENTRY
#define APIENTRY __stdcall
#endif

extern "C" {
void APIENTRY glutInit(int* argcp, char** argv);
void APIENTRY glutInitDisplayMode(unsigned int mode);
void APIENTRY glutInitWindowSize(int width, int height);
void APIENTRY glutInitWindowPosition(int x, int y);
int APIENTRY glutCreateWindow(const char* title);
void APIENTRY glutDisplayFunc(void (*func)(void));
void APIENTRY glutReshapeFunc(void (*func)(int, int));
void APIENTRY glutKeyboardFunc(void (*func)(unsigned char, int, int));
void APIENTRY glutMainLoop(void);
void APIENTRY glutSwapBuffers(void);
void APIENTRY glutPostRedisplay(void);
void APIENTRY glutSolidCube(GLdouble size);
}

struct FingerState {
    GLfloat curl;
    GLfloat spread;
};

struct ColorRGB {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

static int shoulder = 0;
static int elbow = 0;

static GLfloat wristFlex = 0.0f;
static GLfloat wristDeviation = 0.0f;
static GLfloat forearmTwist = 0.0f;

static FingerState thumb = {0.0f, 0.0f};
static FingerState fingers[4] = {
    {0.0f, 0.0f},
    {0.0f, 0.0f},
    {0.0f, 0.0f},
    {0.0f, 0.0f}
};

static GLfloat clampf(GLfloat value, GLfloat minValue, GLfloat maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static ColorRGB shadeColor(const ColorRGB& c, GLfloat factor) {
    ColorRGB out;
    out.r = clampf(c.r * factor, 0.0f, 1.0f);
    out.g = clampf(c.g * factor, 0.0f, 1.0f);
    out.b = clampf(c.b * factor, 0.0f, 1.0f);
    return out;
}

static GLfloat approachf(GLfloat value, GLfloat target, GLfloat step) {
    if (value < target) {
        value += step;
        if (value > target) {
            value = target;
        }
    } else if (value > target) {
        value -= step;
        if (value < target) {
            value = target;
        }
    }
    return value;
}

void init(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void drawSegment(GLfloat sx, GLfloat sy, GLfloat sz, const ColorRGB& color) {
    glPushMatrix();
    glScalef(sx, sy, sz);

    glColor3f(color.r, color.g, color.b);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawPhalanx(GLfloat length, GLfloat height, GLfloat depth, const ColorRGB& color, int withNail) {
    glTranslatef(length * 0.5f, 0.0f, 0.0f);
    drawSegment(length, height, depth, color);

    if (withNail) {
        ColorRGB nailColor = {0.94f, 0.86f, 0.82f};
        glPushMatrix();
        glTranslatef(length * 0.16f, 0.0f, depth * 0.44f);
        drawSegment(length * 0.28f, height * 0.55f, depth * 0.20f, nailColor);
        glPopMatrix();
    }

    glTranslatef(length * 0.5f, 0.0f, 0.0f);
}

void drawFinger(const FingerState& fingerState,
                GLfloat baseX,
                GLfloat baseY,
                GLfloat baseSpread,
                GLfloat lateralBias,
                GLfloat len1,
                GLfloat len2,
                GLfloat len3,
                const ColorRGB& baseColor) {
    glPushMatrix();
    glTranslatef(baseX, baseY, 0.0f);

    glRotatef(baseSpread + fingerState.spread, 0.0f, 0.0f, 1.0f);

    GLfloat mcp = fingerState.curl;
    GLfloat pip = (fingerState.curl >= 0.0f) ? fingerState.curl * 0.72f : fingerState.curl * 0.15f;
    GLfloat dip = (fingerState.curl >= 0.0f) ? fingerState.curl * 0.56f : fingerState.curl * 0.10f;

    glRotatef(mcp, 0.0f, 1.0f, 0.0f);
    drawPhalanx(len1, 0.10f, 0.10f, baseColor, 0);

    glTranslatef(0.0f, lateralBias, 0.0f);
    glRotatef(pip, 0.0f, 1.0f, 0.0f);
    drawPhalanx(len2, 0.09f, 0.09f, shadeColor(baseColor, 0.93f), 0);

    glTranslatef(0.0f, lateralBias * 1.10f, 0.0f);
    glRotatef(dip, 0.0f, 1.0f, 0.0f);
    drawPhalanx(len3, 0.08f, 0.08f, shadeColor(baseColor, 0.88f), 1);
    glPopMatrix();
}

void drawThumb(const FingerState& thumbState, const ColorRGB& baseColor) {
    glPushMatrix();
    glTranslatef(0.18f, -0.30f, 0.0f);

    glRotatef(-38.0f + thumbState.spread, 0.0f, 0.0f, 1.0f);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);

    GLfloat mcp = thumbState.curl;
    GLfloat ip = (thumbState.curl >= 0.0f) ? thumbState.curl * 0.62f : thumbState.curl * 0.18f;

    glRotatef(mcp, 0.0f, 1.0f, 0.0f);
    drawPhalanx(0.25f, 0.11f, 0.11f, baseColor, 0);

    glRotatef(ip, 0.0f, 1.0f, 0.0f);
    drawPhalanx(0.18f, 0.095f, 0.095f, shadeColor(baseColor, 0.90f), 1);
    glPopMatrix();
}

void display(void) {
    static const GLfloat fingerBaseX[4] = {0.39f, 0.40f, 0.39f, 0.37f};
    static const GLfloat fingerBaseY[4] = {-0.23f, -0.07f, 0.10f, 0.28f};
    static const GLfloat fingerBaseSpread[4] = {10.0f, 3.0f, -3.0f, -10.0f};
    static const GLfloat fingerLateralBias[4] = {-0.010f, -0.003f, 0.003f, 0.010f};
    static const GLfloat fingerLen1[4] = {0.30f, 0.34f, 0.32f, 0.28f};
    static const GLfloat fingerLen2[4] = {0.22f, 0.25f, 0.23f, 0.20f};
    static const GLfloat fingerLen3[4] = {0.17f, 0.19f, 0.18f, 0.16f};

    static const ColorRGB upperArmColor = {0.95f, 0.40f, 0.36f};
    static const ColorRGB foreArmColor = {0.98f, 0.68f, 0.25f};
    static const ColorRGB palmColor = {0.99f, 0.90f, 0.28f};
    static const ColorRGB palmMarkerColor = {0.17f, 0.70f, 0.72f};
    static const ColorRGB thumbColor = {0.29f, 0.59f, 0.96f};
    static const ColorRGB fingerColors[4] = {
        {0.61f, 0.35f, 0.71f},
        {0.31f, 0.78f, 0.47f},
        {0.99f, 0.41f, 0.71f},
        {0.00f, 0.74f, 0.83f}
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 0.0f);
        glRotatef((GLfloat) shoulder, 0.0f, 0.0f, 1.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);
        drawSegment(2.0f, 0.40f, 1.0f, upperArmColor);

        glTranslatef(1.0f, 0.0f, 0.0f);
        glRotatef((GLfloat) elbow, 0.0f, 0.0f, 1.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);
        drawSegment(2.0f, 0.35f, 0.8f, foreArmColor);

        glTranslatef(1.0f, 0.0f, 0.0f);
        glRotatef(forearmTwist, 1.0f, 0.0f, 0.0f);
        glRotatef(wristDeviation, 0.0f, 1.0f, 0.0f);
        glRotatef(wristFlex, 0.0f, 0.0f, 1.0f);

        glTranslatef(0.39f, 0.0f, 0.0f);
        drawSegment(0.78f, 0.60f, 0.30f, palmColor);
        glPushMatrix();
        glTranslatef(0.20f, 0.0f, 0.18f);
        drawSegment(0.22f, 0.26f, 0.03f, palmMarkerColor);
        glPopMatrix();

        drawThumb(thumb, thumbColor);
        drawFinger(fingers[0], fingerBaseX[0], fingerBaseY[0], fingerBaseSpread[0], fingerLateralBias[0], fingerLen1[0], fingerLen2[0], fingerLen3[0], fingerColors[0]);
        drawFinger(fingers[1], fingerBaseX[1], fingerBaseY[1], fingerBaseSpread[1], fingerLateralBias[1], fingerLen1[1], fingerLen2[1], fingerLen3[1], fingerColors[1]);
        drawFinger(fingers[2], fingerBaseX[2], fingerBaseY[2], fingerBaseSpread[2], fingerLateralBias[2], fingerLen1[2], fingerLen2[2], fingerLen3[2], fingerColors[2]);
        drawFinger(fingers[3], fingerBaseX[3], fingerBaseY[3], fingerBaseSpread[3], fingerLateralBias[3], fingerLen1[3], fingerLen2[3], fingerLen3[3], fingerColors[3]);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) {
        h = 1;
    }

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.5f);
}

void rotateJoint(int* joint, int delta) {
    *joint = (*joint + delta) % 360;
    if (*joint < 0) {
        *joint += 360;
    }
}

void rotateWrist(GLfloat* angle, GLfloat delta, GLfloat minValue, GLfloat maxValue) {
    *angle = clampf(*angle + delta, minValue, maxValue);
}

void bendThumb(GLfloat delta) {
    thumb.curl = clampf(thumb.curl + delta, -22.0f, 85.0f);
}

void spreadThumb(GLfloat delta) {
    thumb.spread = clampf(thumb.spread + delta, -28.0f, 32.0f);
}

void bendFinger(int index, GLfloat delta) {
    fingers[index].curl = clampf(fingers[index].curl + delta, -25.0f, 110.0f);
}

void fanFingers(GLfloat delta) {
    fingers[0].spread = clampf(fingers[0].spread + delta, -28.0f, 28.0f);
    fingers[1].spread = clampf(fingers[1].spread + delta * 0.6f, -20.0f, 20.0f);
    fingers[2].spread = clampf(fingers[2].spread - delta * 0.6f, -20.0f, 20.0f);
    fingers[3].spread = clampf(fingers[3].spread - delta, -28.0f, 28.0f);
}

void grip(GLfloat delta) {
    bendThumb(delta * 0.70f);
    bendFinger(0, delta * 0.92f);
    bendFinger(1, delta * 1.00f);
    bendFinger(2, delta * 1.08f);
    bendFinger(3, delta * 1.15f);

    if (delta > 0.0f) {
        fingers[0].spread = approachf(fingers[0].spread, 1.5f, 1.8f);
        fingers[1].spread = approachf(fingers[1].spread, 0.5f, 1.8f);
        fingers[2].spread = approachf(fingers[2].spread, -0.5f, 1.8f);
        fingers[3].spread = approachf(fingers[3].spread, -1.5f, 1.8f);
        thumb.spread = approachf(thumb.spread, -10.0f, 2.0f);
    } else {
        thumb.spread = approachf(thumb.spread, 0.0f, 1.5f);
    }
}

void straightenFingers(void) {
    thumb.curl = 0.0f;
    fingers[0].curl = 0.0f;
    fingers[1].curl = 0.0f;
    fingers[2].curl = 0.0f;
    fingers[3].curl = 0.0f;
}

void resetPose(void) {
    shoulder = 0;
    elbow = 0;
    wristFlex = 0.0f;
    wristDeviation = 0.0f;
    forearmTwist = 0.0f;

    thumb.curl = 0.0f;
    thumb.spread = 0.0f;

    fingers[0].curl = 0.0f;
    fingers[1].curl = 0.0f;
    fingers[2].curl = 0.0f;
    fingers[3].curl = 0.0f;

    fingers[0].spread = 0.0f;
    fingers[1].spread = 0.0f;
    fingers[2].spread = 0.0f;
    fingers[3].spread = 0.0f;
}

void keyboard(unsigned char key, int x, int y) {
    (void) x;
    (void) y;

    switch (key) {
        // s/S: bahu
        case 's':
            rotateJoint(&shoulder, 5);
            break;
        case 'S':
            rotateJoint(&shoulder, -5);
            break;

        // e/E: siku
        case 'e':
            rotateJoint(&elbow, 5);
            break;
        case 'E':
            rotateJoint(&elbow, -5);
            break;

        // w/W: wrist flex-extend
        case 'w':
            rotateWrist(&wristFlex, 5.0f, -80.0f, 80.0f);
            break;
        case 'W':
            rotateWrist(&wristFlex, -5.0f, -80.0f, 80.0f);
            break;

        // q/Q: wrist deviation
        case 'q':
            rotateWrist(&wristDeviation, 4.0f, -40.0f, 40.0f);
            break;
        case 'Q':
            rotateWrist(&wristDeviation, -4.0f, -40.0f, 40.0f);
            break;

        // a/A atau p/P: supinasi-pronasi
        case 'a':
        case 'p':
            rotateWrist(&forearmTwist, 6.0f, -120.0f, 120.0f);
            break;
        case 'A':
        case 'P':
            rotateWrist(&forearmTwist, -6.0f, -120.0f, 120.0f);
            break;

        // t/T: tekuk ibu jari
        case 't':
            bendThumb(5.0f);
            break;
        case 'T':
            bendThumb(-5.0f);
            break;

        // y/Y: buka-tutup ibu jari
        case 'y':
            spreadThumb(3.0f);
            break;
        case 'Y':
            spreadThumb(-3.0f);
            break;

        // i/I m/M r/R k/K: tekuk tiap jari
        case 'i':
            bendFinger(0, 5.0f);
            break;
        case 'I':
            bendFinger(0, -5.0f);
            break;
        case 'm':
            bendFinger(1, 5.0f);
            break;
        case 'M':
            bendFinger(1, -5.0f);
            break;
        case 'r':
            bendFinger(2, 5.0f);
            break;
        case 'R':
            bendFinger(2, -5.0f);
            break;
        case 'k':
            bendFinger(3, 5.0f);
            break;
        case 'K':
            bendFinger(3, -5.0f);
            break;

        // u/U: sebar-rapat jari
        case 'u':
            fanFingers(3.0f);
            break;
        case 'U':
            fanFingers(-3.0f);
            break;

        // g/G: genggam-buka tangan
        case 'g':
            grip(6.0f);
            break;
        case 'G':
            grip(-6.0f);
            break;

        // h/H: luruskan jari
        case 'h':
        case 'H':
            straightenFingers();
            break;

        // 0: reset pose
        case '0':
            resetPose();
            break;

        // ESC: keluar
        case 27:
            exit(0);
            return;

        default:
            return;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    puts("Kontrol:");
    puts("s/S: bahu | e/E: siku");
    puts("w/W: wrist flex-extend | q/Q: wrist deviation");
    puts("a/A atau p/P: supinasi-pronasi");
    puts("t/T: tekuk ibu jari | y/Y: buka-tutup ibu jari");
    puts("i/I m/M r/R k/K: tekuk tiap jari");
    puts("u/U: sebar-rapat jari | g/G: genggam-buka tangan | h/H: luruskan jari");
    puts("0: reset | ESC: keluar");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 650);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lengan Bergerak + Telapak Tangan + Jari [BERWARNA]");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

