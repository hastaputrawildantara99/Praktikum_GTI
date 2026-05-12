// Dev-C++/GLUT lama memiliki atexit hack yang bisa bentrok saat compile.
// Baris ini membuat header GLUT lebih stabil untuk project ini.
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <math.h>
#include <cstdlib>
#include <cstring>

const int WIDTH = 1000;
const int HEIGHT = 700;

// Kecepatan kamera bergerak dan menoleh saat tombol arrow ditekan.
const float CAMERA_MOVE_SPEED = 9.0f;
const float CAMERA_TURN_SPEED = 1.6f;

enum ProjectionMode {
    PROJ_ORTHO = 0,
    PROJ_1POINT = 1,
    PROJ_2POINT = 2,
    PROJ_3POINT = 3
};

struct Character {
    float x, y, z;
    float baseY;
    float hp;
    float bodyR, bodyG, bodyB;
    bool hit;
    float hitTimer;
    float idleTime;
    float knockback;
    float attackAnim;
    bool dead;
    float fallAngle;
    float fallOffsetY;
};

struct Projectile {
    bool active;
    float x, y, z;
    float vx, vy, vz;
    float r, g, b;
    int owner;
};

Character leftChar;
Character rightChar;
Projectile projectile;

ProjectionMode projectionMode = PROJ_1POINT;
GLenum shadingMode = GL_SMOOTH;

float globalTime = 0.0f;
float cameraShake = 0.0f;
float shockwaveTimer = 0.0f;
float shockwaveX = 0.0f;
float shockwaveY = 0.0f;
float shockwaveZ = 0.0f;

// State movable camera: posisi kamera, arah hadap, dan arah input keyboard.
// cameraYaw dipakai untuk menoleh kiri/kanan, cameraPitch untuk arah naik/turun kamera.
float cameraX = 0.0f;
float cameraY = 6.0f;
float cameraZ = 26.0f;
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
int cameraMoveDirection = 0;
int cameraTurnDirection = 0;

// Properti cahaya dan material mengikuti konsep modul Depth dan Lighting.
// Komponen ambient, diffuse, specular, dan posisi cahaya dikirim ke GL_LIGHT0.
const GLfloat light_ambient[]  = { 0.50f, 0.50f, 0.50f, 1.0f };
const GLfloat light_diffuse[]  = { 1.00f, 1.00f, 1.00f, 1.0f };
const GLfloat light_specular[] = { 1.00f, 1.00f, 1.00f, 1.0f };
const GLfloat light_position[] = { 12.0f, 18.0f, 10.0f, 1.0f };

const GLfloat mat_ambient[]    = { 0.70f, 0.70f, 0.70f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.80f, 0.80f, 0.80f, 1.0f };
const GLfloat mat_specular[]   = { 1.00f, 1.00f, 1.00f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

// Mengatur posisi kamera dan menghitung arah hadapnya dari titik target.
// Tombol 1-4 memakai fungsi ini sebagai preset awal sebelum kamera digerakkan arrow key.
void setCameraView(float eyeX, float eyeY, float eyeZ,
                   float targetX, float targetY, float targetZ) {
    cameraX = eyeX;
    cameraY = eyeY;
    cameraZ = eyeZ;

    float dx = targetX - eyeX;
    float dy = targetY - eyeY;
    float dz = targetZ - eyeZ;
    float horizontalDistance = sqrt(dx * dx + dz * dz);

    cameraYaw = atan2(dx, -dz);
    cameraPitch = atan2(dy, horizontalDistance);
    cameraMoveDirection = 0;
    cameraTurnDirection = 0;
}

// Preset sudut pandang lama tetap dipakai, tetapi sekarang menjadi posisi awal movable camera.
void setCameraPreset(ProjectionMode mode) {
    if (mode == PROJ_ORTHO) {
        setCameraView(0.0f, 10.0f, 22.0f, 0.0f, 2.0f, 0.0f);
    } else if (mode == PROJ_1POINT) {
        setCameraView(0.0f, 6.0f, 26.0f, 0.0f, 2.0f, 0.0f);
    } else if (mode == PROJ_2POINT) {
        setCameraView(20.0f, 7.0f, 20.0f, 0.0f, 2.0f, 0.0f);
    } else if (mode == PROJ_3POINT) {
        setCameraView(15.0f, 22.0f, 15.0f, 0.0f, 1.5f, 0.0f);
    }
}

// Membatasi kamera agar tidak terlalu jauh keluar dari arena kota.
void clampCameraPosition() {
    if (cameraX < -28.0f) cameraX = -28.0f;
    if (cameraX >  28.0f) cameraX =  28.0f;
    if (cameraZ < -30.0f) cameraZ = -30.0f;
    if (cameraZ >  35.0f) cameraZ =  35.0f;
}

// Dipanggil setiap frame. Jika arrow key ditahan, kamera akan maju/mundur atau menoleh.
void updateMovableCamera(float dt) {
    if (cameraTurnDirection != 0) {
        cameraYaw += cameraTurnDirection * CAMERA_TURN_SPEED * dt;
    }

    if (cameraMoveDirection != 0) {
        float step = cameraMoveDirection * CAMERA_MOVE_SPEED * dt;
        cameraX += sin(cameraYaw) * step;
        cameraZ -= cos(cameraYaw) * step;
        clampCameraPosition();
    }
}

// Material per objek digunakan agar masing-masing model punya warna dan kilap berbeda.
void setMaterial(float ar, float ag, float ab,
                 float dr, float dg, float db,
                 float sr, float sg, float sb,
                 float shininess) {
    GLfloat ambient[]  = { ar, ag, ab, 1.0f };
    GLfloat diffuse[]  = { dr, dg, db, 1.0f };
    GLfloat specular[] = { sr, sg, sb, 1.0f };
    GLfloat shine[]    = { shininess };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

// Setup Depth dan Lighting seperti pada modul.
// Depth test membuat objek dekat menutup objek jauh, sedangkan lighting memberi efek cahaya.
void lighting() {
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // Project ini memakai glMaterialfv() lewat setMaterial().
    // Karena itu GL_COLOR_MATERIAL dimatikan agar warna glColor3f() tidak merusak material objek.
    glDisable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
}

void drawBitmapText(float x, float y, const char *text) {
    int len = (int)strlen(text);
    int i;
    glRasterPos2f(x, y);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void initCharacters() {
    leftChar.x = -8.0f;
    leftChar.y = 1.2f;
    leftChar.z = 0.0f;
    leftChar.baseY = 1.2f;
    leftChar.hp = 100.0f;
    leftChar.bodyR = 0.95f;
    leftChar.bodyG = 0.30f;
    leftChar.bodyB = 0.15f;
    leftChar.hit = false;
    leftChar.hitTimer = 0.0f;
    leftChar.idleTime = 0.0f;
    leftChar.knockback = 0.0f;
    leftChar.attackAnim = 0.0f;
    leftChar.dead = false;
    leftChar.fallAngle = 0.0f;
    leftChar.fallOffsetY = 0.0f;

    rightChar.x = 8.0f;
    rightChar.y = 1.2f;
    rightChar.z = 0.0f;
    rightChar.baseY = 1.2f;
    rightChar.hp = 100.0f;
    rightChar.bodyR = 0.20f;
    rightChar.bodyG = 0.45f;
    rightChar.bodyB = 0.95f;
    rightChar.hit = false;
    rightChar.hitTimer = 0.0f;
    rightChar.idleTime = 0.0f;
    rightChar.knockback = 0.0f;
    rightChar.attackAnim = 0.0f;
    rightChar.dead = false;
    rightChar.fallAngle = 0.0f;
    rightChar.fallOffsetY = 0.0f;

    projectile.active = false;
    projectile.x = 0.0f;
    projectile.y = 0.0f;
    projectile.z = 0.0f;
    projectile.vx = 0.0f;
    projectile.vy = 0.0f;
    projectile.vz = 0.0f;
    projectile.r = 1.0f;
    projectile.g = 1.0f;
    projectile.b = 1.0f;
    projectile.owner = 0;

    cameraShake = 0.0f;
    shockwaveTimer = 0.0f;
}

void drawSkyBackdrop() {
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);
        glColor3f(0.30f, 0.65f, 1.00f);
        glVertex3f(-45.0f, 28.0f, -28.0f);
        glVertex3f( 45.0f, 28.0f, -28.0f);
        glColor3f(0.85f, 0.95f, 1.00f);
        glVertex3f( 45.0f,  0.0f, -28.0f);
        glVertex3f(-45.0f,  0.0f, -28.0f);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.55f);
    glPushMatrix();
    glTranslatef(20.0f, 19.0f, -27.0f);
    glutSolidSphere(1.8f, 24, 24);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-18.0f, 17.0f, -27.0f);
    glutSolidSphere(1.0f, 16, 16);
    glTranslatef(1.2f, 0.3f, 0.0f);
    glutSolidSphere(1.1f, 16, 16);
    glTranslatef(1.2f, -0.2f, 0.0f);
    glutSolidSphere(1.0f, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.0f, 15.5f, -27.0f);
    glutSolidSphere(0.9f, 16, 16);
    glTranslatef(1.0f, 0.2f, 0.0f);
    glutSolidSphere(1.0f, 16, 16);
    glTranslatef(1.0f, -0.15f, 0.0f);
    glutSolidSphere(0.9f, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawGround() {
    setMaterial(0.22f, 0.22f, 0.24f,
                0.40f, 0.40f, 0.44f,
                0.12f, 0.12f, 0.12f,
                18.0f);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex3f(-30.0f, 0.0f, -20.0f);
        glVertex3f( 30.0f, 0.0f, -20.0f);
        glVertex3f( 30.0f, 0.0f,  20.0f);
        glVertex3f(-30.0f, 0.0f,  20.0f);
    glEnd();

    glDisable(GL_LIGHTING);

    glColor3f(0.72f, 0.72f, 0.74f);
    glBegin(GL_QUADS);
        glVertex3f(-30.0f, 0.01f, -20.0f);
        glVertex3f(-22.0f, 0.01f, -20.0f);
        glVertex3f(-22.0f, 0.01f,  20.0f);
        glVertex3f(-30.0f, 0.01f,  20.0f);
    glEnd();

    glBegin(GL_QUADS);
        glVertex3f(22.0f, 0.01f, -20.0f);
        glVertex3f(30.0f, 0.01f, -20.0f);
        glVertex3f(30.0f, 0.01f,  20.0f);
        glVertex3f(22.0f, 0.01f,  20.0f);
    glEnd();

    glColor3f(0.95f, 0.85f, 0.15f);
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, 0.02f, -18.0f);
        glVertex3f( 1.0f, 0.02f, -18.0f);
        glVertex3f( 1.0f, 0.02f,  18.0f);
        glVertex3f(-1.0f, 0.02f,  18.0f);
    glEnd();

    glColor3f(0.96f, 0.96f, 0.96f);
    int i;
    for (i = -8; i <= 8; i += 2) {
        glBegin(GL_QUADS);
            glVertex3f(-2.8f, 0.02f, (float)i);
            glVertex3f( 2.8f, 0.02f, (float)i);
            glVertex3f( 2.8f, 0.02f, (float)i + 0.7f);
            glVertex3f(-2.8f, 0.02f, (float)i + 0.7f);
        glEnd();
    }

    glEnable(GL_LIGHTING);
}

void drawBuilding(float x, float z, float w, float h, float d,
                  float r, float g, float b, float rotY) {
    glPushMatrix();
    glTranslatef(x, h / 2.0f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(w, h, d);

    setMaterial(r * 0.5f, g * 0.5f, b * 0.5f,
                r, g, b,
                0.55f, 0.55f, 0.60f,
                28.0f);

    glutSolidCube(1.0f);
    glPopMatrix();

    glDisable(GL_LIGHTING);

    float wy, wx;
    float halfW = w / 2.0f;
    float halfD = d / 2.0f;
    float rad = rotY * 3.1415926f / 180.0f;
    float c = cos(rad);
    float s = sin(rad);

    glColor3f(0.72f, 0.88f, 1.0f);
    for (wy = 1.0f; wy < h - 1.0f; wy += 1.5f) {
        for (wx = -halfW + 0.35f; wx < halfW - 0.2f; wx += 0.75f) {
            float x1 = wx;
            float x2 = wx + 0.30f;
            float y1 = wy;
            float y2 = wy + 0.55f;
            float zf = halfD + 0.01f;

            float px1 = x + x1 * c + zf * s;
            float pz1 = z + (-x1 * s + zf * c);
            float px2 = x + x2 * c + zf * s;
            float pz2 = z + (-x2 * s + zf * c);

            glBegin(GL_QUADS);
                glVertex3f(px1, y1, pz1);
                glVertex3f(px2, y1, pz2);
                glVertex3f(px2, y2, pz2);
                glVertex3f(px1, y2, pz1);
            glEnd();
        }
    }

    glColor3f(0.82f, 0.93f, 1.0f);
    for (wy = 1.0f; wy < h - 1.0f; wy += 1.5f) {
        for (wx = -halfW + 0.35f; wx < halfW - 0.2f; wx += 0.75f) {
            float x1 = wx;
            float x2 = wx + 0.30f;
            float y1 = wy;
            float y2 = wy + 0.55f;
            float zb = -halfD - 0.01f;

            float px1 = x + x1 * c + zb * s;
            float pz1 = z + (-x1 * s + zb * c);
            float px2 = x + x2 * c + zb * s;
            float pz2 = z + (-x2 * s + zb * c);

            glBegin(GL_QUADS);
                glVertex3f(px1, y1, pz1);
                glVertex3f(px2, y1, pz2);
                glVertex3f(px2, y2, pz2);
                glVertex3f(px1, y2, pz1);
            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
}

void drawStreetLamp(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    setMaterial(0.20f, 0.20f, 0.20f,
                0.38f, 0.38f, 0.38f,
                0.45f, 0.45f, 0.45f,
                30.0f);

    glPushMatrix();
    glTranslatef(0.0f, 2.5f, 0.0f);
    glScalef(0.15f, 5.0f, 0.15f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45f, 4.8f, 0.0f);
    glScalef(0.9f, 0.12f, 0.12f);
    glutSolidCube(1.0f);
    glPopMatrix();

    setMaterial(0.75f, 0.75f, 0.60f,
                0.95f, 0.95f, 0.75f,
                0.85f, 0.85f, 0.80f,
                50.0f);

    glPushMatrix();
    glTranslatef(0.85f, 4.8f, 0.0f);
    glutSolidSphere(0.18f, 12, 12);
    glPopMatrix();

    glPopMatrix();
}

void drawArena() {
    drawSkyBackdrop();
    drawGround();

    drawBuilding(-20.0f, -14.0f, 4.0f, 12.0f, 4.0f, 0.70f, 0.74f, 0.80f, -6.0f);
    drawBuilding(-13.0f, -15.0f, 5.0f, 16.0f, 4.5f, 0.62f, 0.68f, 0.76f,  4.0f);
    drawBuilding(-5.0f,  -14.0f, 6.0f, 11.0f, 4.0f, 0.76f, 0.78f, 0.84f, -5.0f);
    drawBuilding( 4.0f,  -15.0f, 5.0f, 18.0f, 4.5f, 0.66f, 0.70f, 0.78f,  6.0f);
    drawBuilding(12.0f,  -14.0f, 4.5f, 13.0f, 4.0f, 0.72f, 0.76f, 0.82f, -4.0f);
    drawBuilding(20.0f,  -15.0f, 5.0f, 15.0f, 4.5f, 0.60f, 0.66f, 0.74f,  5.0f);

    drawBuilding(-24.0f, -4.0f, 3.5f, 10.0f, 3.5f, 0.68f, 0.72f, 0.78f,  8.0f);
    drawBuilding( 24.0f, -3.0f, 3.5f, 10.0f, 3.5f, 0.68f, 0.72f, 0.78f, -8.0f);

    drawStreetLamp(-10.0f, 4.0f);
    drawStreetLamp(10.0f, 4.0f);
    drawStreetLamp(-10.0f, -8.0f);
    drawStreetLamp(10.0f, -8.0f);
}

void drawShockwave() {
    if (shockwaveTimer <= 0.0f) return;

    float radius = 0.8f + (1.0f - shockwaveTimer) * 2.5f;

    glDisable(GL_LIGHTING);
    if (projectile.owner == 1) glColor3f(1.0f, 0.65f, 0.20f);
    else glColor3f(1.0f, 1.0f, 0.30f);

    glPushMatrix();
    glTranslatef(shockwaveX, shockwaveY, shockwaveZ);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutWireTorus(0.05f, radius, 12, 40);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void drawProjectile() {
    if (!projectile.active) return;

    setMaterial(projectile.r * 0.3f, projectile.g * 0.3f, projectile.b * 0.3f,
                projectile.r, projectile.g, projectile.b,
                1.0f, 1.0f, 1.0f,
                80.0f);

    glPushMatrix();
    glTranslatef(projectile.x, projectile.y, projectile.z);
    glutSolidSphere(0.35f, 18, 18);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.9f);
    glBegin(GL_LINES);
        glVertex3f(projectile.x, projectile.y, projectile.z);
        glVertex3f(projectile.x - projectile.vx * 1.8f,
                   projectile.y - projectile.vy * 1.8f,
                   projectile.z - projectile.vz * 1.8f);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawCharacter(Character &c, int facing) {
    float br = c.bodyR;
    float bg = c.bodyG;
    float bb = c.bodyB;

    if (c.hit) {
        if (fmod(c.hitTimer * 18.0f, 2.0f) > 1.0f) {
            br = 1.0f;
            bg = 1.0f;
            bb = 1.0f;
        }
    }

    glPushMatrix();
    glTranslatef(c.x, c.y + c.fallOffsetY, c.z);

    if (!c.dead) {
        if (facing == 1 && c.attackAnim > 0.0f) glTranslatef(c.attackAnim, 0.0f, 0.0f);
        if (facing == -1 && c.attackAnim > 0.0f) glTranslatef(-c.attackAnim, 0.0f, 0.0f);
    }

    if (c.dead) {
        if (facing == 1) glRotatef(-c.fallAngle, 0.0f, 0.0f, 1.0f);
        else glRotatef(c.fallAngle, 0.0f, 0.0f, 1.0f);
    }

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -1.1f, 0.0f);
    if (c.dead) glScalef(2.0f, 0.08f, 1.2f);
    else glScalef(1.7f, 0.10f, 1.0f);
    glutSolidSphere(1.0f, 16, 16);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    setMaterial(br * 0.4f, bg * 0.4f, bb * 0.4f,
                br, bg, bb,
                0.8f, 0.8f, 0.8f,
                40.0f);

    glPushMatrix();
    glScalef(1.5f, 1.2f, 1.0f);
    glutSolidCube(1.6f);
    glPopMatrix();

    setMaterial(br * 0.45f, bg * 0.45f, bb * 0.45f,
                br * 0.95f + 0.05f, bg * 0.95f + 0.05f, bb * 0.95f + 0.05f,
                0.9f, 0.9f, 0.9f,
                60.0f);

    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);
    glutSolidSphere(0.75f, 20, 20);
    glPopMatrix();

    setMaterial(0.15f, 0.08f, 0.03f,
                0.30f, 0.18f, 0.08f,
                0.2f, 0.2f, 0.2f,
                10.0f);

    glPushMatrix();
    glTranslatef(-0.5f, -1.0f, 0.35f);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5f, -1.0f, 0.35f);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5f, -1.0f, -0.35f);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5f, -1.0f, -0.35f);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();

    if (facing == 1) {
        setMaterial(0.8f, 0.3f, 0.0f,
                    1.0f, 0.5f, 0.1f,
                    1.0f, 0.8f, 0.5f,
                    70.0f);

        glPushMatrix();
        glTranslatef(-1.4f, 0.3f, 0.0f);
        glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
        glutSolidCone(0.35f, 1.4f, 12, 12);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.25f, 2.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.18f, 0.45f, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.25f, 2.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.18f, 0.45f, 10, 10);
        glPopMatrix();
    } else {
        setMaterial(0.7f, 0.7f, 0.0f,
                    1.0f, 1.0f, 0.2f,
                    1.0f, 1.0f, 0.6f,
                    70.0f);

        glPushMatrix();
        glTranslatef(1.4f, 0.3f, 0.0f);
        glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
        glutSolidCone(0.35f, 1.4f, 12, 12);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.20f, 2.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.16f, 0.42f, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.20f, 2.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.16f, 0.42f, 10, 10);
        glPopMatrix();
    }

    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    if (facing == 1) glTranslatef(0.22f, 1.35f, 0.65f);
    else glTranslatef(-0.22f, 1.35f, 0.65f);
    glutSolidSphere(0.07f, 10, 10);
    glPopMatrix();

    if (c.hit && !c.dead) {
        int i;
        if (facing == 1) glColor3f(1.0f, 0.8f, 0.3f);
        else glColor3f(1.0f, 1.0f, 0.3f);

        for (i = 0; i < 8; i++) {
            float a = i * 45.0f * 3.1415926f / 180.0f + globalTime * 6.0f;
            glBegin(GL_LINES);
                glVertex3f(0.0f, 0.8f, 0.0f);
                glVertex3f(cos(a) * 1.0f, 0.8f + sin(a) * 1.0f, sin(a) * 0.8f);
            glEnd();
        }
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void setProjection() {
    // GL_PROJECTION mengatur jenis proyeksi: orthographic atau perspective.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (projectionMode == PROJ_ORTHO) {
        glOrtho(-18.0, 18.0, -10.0, 10.0, 1.0, 100.0);
    } else {
        gluPerspective(60.0, (double)WIDTH / (double)HEIGHT, 1.0, 100.0);
    }

    // GL_MODELVIEW mengatur view/kamera. Posisi dan arah kamera dikirim ke gluLookAt().
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float sx = 0.0f;
    float sy = 0.0f;
    if (cameraShake > 0.0f) {
        sx = sin(globalTime * 80.0f) * 0.25f * cameraShake;
        sy = cos(globalTime * 90.0f) * 0.18f * cameraShake;
    }

    float cosPitch = cos(cameraPitch);
    float lookX = sin(cameraYaw) * cosPitch;
    float lookY = sin(cameraPitch);
    float lookZ = -cos(cameraYaw) * cosPitch;

    // Kamera bergerak memakai cameraX/Y/Z sebagai eye position,
    // lalu melihat ke arah vektor lookX/lookY/lookZ.
    gluLookAt(
        cameraX + sx,       cameraY + sy,       cameraZ,
        cameraX + lookX,    cameraY + lookY,    cameraZ + lookZ,
        0.0,                1.0,                0.0
    );
}

void setupLighting() {
    // Cahaya dasar diambil dari variabel modul, lalu diffuse sedikit diubah saat projectile aktif.
    // Ini membuat efek serangan terasa, tanpa menghilangkan konsep GL_LIGHT0.
    GLfloat diffuse[] = {
        light_diffuse[0],
        light_diffuse[1],
        light_diffuse[2],
        light_diffuse[3]
    };

    if (projectile.active) {
        if (projectile.owner == 1) {
            diffuse[0] = 1.0f; diffuse[1] = 0.82f; diffuse[2] = 0.68f;
        } else {
            diffuse[0] = 0.82f; diffuse[1] = 0.90f; diffuse[2] = 1.0f;
        }
    }

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void begin2DOverlay() {
    // Overlay 2D untuk teks/HP bar tidak memakai depth dan lighting agar selalu terbaca.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

void end2DOverlay() {
    // Setelah menggambar overlay, depth dan lighting diaktifkan lagi untuk render 3D berikutnya.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void drawHPBar(float x, float y, float hp, const char *name) {
    float fill = (hp / 100.0f) * 210.0f;

    glColor3f(0.08f, 0.08f, 0.08f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + 220.0f, y);
        glVertex2f(x + 220.0f, y + 28.0f);
        glVertex2f(x, y + 28.0f);
    glEnd();

    glColor3f(0.60f, 0.12f, 0.12f);
    glBegin(GL_QUADS);
        glVertex2f(x + 5.0f, y + 5.0f);
        glVertex2f(x + 215.0f, y + 5.0f);
        glVertex2f(x + 215.0f, y + 23.0f);
        glVertex2f(x + 5.0f, y + 23.0f);
    glEnd();

    glColor3f(0.15f, 0.85f, 0.20f);
    glBegin(GL_QUADS);
        glVertex2f(x + 5.0f, y + 5.0f);
        glVertex2f(x + 5.0f + fill, y + 5.0f);
        glVertex2f(x + 5.0f + fill, y + 23.0f);
        glVertex2f(x + 5.0f, y + 23.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(x, y + 35.0f, name);
}

void drawOverlay() {
    begin2DOverlay();

    drawHPBar(30.0f, 640.0f, leftChar.hp, "Flame Beast");
    drawHPBar(750.0f, 640.0f, rightChar.hp, "Thunder Beast");

    glColor3f(1.0f, 1.0f, 1.0f);  

    if (shadingMode == GL_FLAT)
        drawBitmapText(20.0f, 45.0f, "Shading: Flat");
    else
        drawBitmapText(20.0f, 45.0f, "Shading: Smooth");

    drawBitmapText(20.0f, 95.0f, "Depth: GL_DEPTH_TEST | Lighting: GL_LIGHT0 + Material");
    drawBitmapText(20.0f, 70.0f, "Camera: Arrow Up/Down maju-mundur | Arrow Left/Right tengok");
    drawBitmapText(20.0f, 20.0f, "A/L attack | 1 ortho | 2 1VP | 3 2VP | 4 3VP | F flat | G smooth | R reset");

    if (leftChar.hp <= 0.0f) {
        glColor3f(0.9f, 0.9f, 0.2f);
        drawBitmapText(420.0f, 360.0f, "Thunder Beast Menang!");
    } else if (rightChar.hp <= 0.0f) {
        glColor3f(0.9f, 0.9f, 0.2f);
        drawBitmapText(430.0f, 360.0f, "Flame Beast Menang!");
    }

    end2DOverlay();
}

void attackLeft() {
    if (projectile.active || leftChar.hp <= 0.0f || rightChar.hp <= 0.0f) return;
    if (leftChar.dead || rightChar.dead) return;

    projectile.active = true;
    projectile.owner = 1;
    projectile.x = leftChar.x + 1.8f;
    projectile.y = leftChar.y + 1.2f;
    projectile.z = leftChar.z;
    projectile.vx = 0.35f;
    projectile.vy = 0.0f;
    projectile.vz = 0.0f;
    projectile.r = 1.0f;
    projectile.g = 0.45f;
    projectile.b = 0.10f;

    leftChar.attackAnim = 0.35f;
}

void attackRight() {
    if (projectile.active || leftChar.hp <= 0.0f || rightChar.hp <= 0.0f) return;
    if (leftChar.dead || rightChar.dead) return;

    projectile.active = true;
    projectile.owner = 2;
    projectile.x = rightChar.x - 1.8f;
    projectile.y = rightChar.y + 1.2f;
    projectile.z = rightChar.z;
    projectile.vx = -0.35f;
    projectile.vy = 0.0f;
    projectile.vz = 0.0f;
    projectile.r = 1.0f;
    projectile.g = 1.0f;
    projectile.b = 0.25f;

    rightChar.attackAnim = 0.35f;
}

void resetBattle() {
    initCharacters();
}

void updateFalling(Character &c) {
    if (!c.dead) return;

    if (c.fallAngle < 90.0f) {
        c.fallAngle += 2.5f;
        if (c.fallAngle > 90.0f) c.fallAngle = 90.0f;
    }

    if (c.fallOffsetY > -0.8f) {
        c.fallOffsetY -= 0.03f;
        if (c.fallOffsetY < -0.8f) c.fallOffsetY = -0.8f;
    }
}

void update(int value) {
    float dt = 0.016f;
    globalTime += dt;

    // Update posisi kamera berdasarkan input arrow key yang sedang ditahan.
    updateMovableCamera(dt);

    leftChar.idleTime += dt;
    rightChar.idleTime += dt;

    if (!leftChar.dead)
        leftChar.y = leftChar.baseY + sin(leftChar.idleTime * 3.0f) * 0.10f;
    if (!rightChar.dead)
        rightChar.y = rightChar.baseY + sin(rightChar.idleTime * 3.0f + 1.0f) * 0.10f;

    if (leftChar.hit) {
        leftChar.hitTimer -= dt;
        if (leftChar.hitTimer <= 0.0f) leftChar.hit = false;
    }

    if (rightChar.hit) {
        rightChar.hitTimer -= dt;
        if (rightChar.hitTimer <= 0.0f) rightChar.hit = false;
    }

    if (!leftChar.dead) {
        if (leftChar.knockback > 0.0f) {
            leftChar.x += 0.08f;
            leftChar.knockback -= dt * 3.5f;
        } else if (leftChar.x > -8.0f) {
            leftChar.x -= 0.05f;
        }
    }

    if (!rightChar.dead) {
        if (rightChar.knockback > 0.0f) {
            rightChar.x -= 0.08f;
            rightChar.knockback -= dt * 3.5f;
        } else if (rightChar.x < 8.0f) {
            rightChar.x += 0.05f;
        }
    }

    if (leftChar.attackAnim > 0.0f) {
        leftChar.attackAnim -= dt * 2.5f;
        if (leftChar.attackAnim < 0.0f) leftChar.attackAnim = 0.0f;
    }

    if (rightChar.attackAnim > 0.0f) {
        rightChar.attackAnim -= dt * 2.5f;
        if (rightChar.attackAnim < 0.0f) rightChar.attackAnim = 0.0f;
    }

    if (projectile.active) {
        projectile.x += projectile.vx;
        projectile.y += projectile.vy;
        projectile.z += projectile.vz;

        if (projectile.owner == 1 && projectile.x >= rightChar.x - 1.0f) {
            projectile.active = false;
            rightChar.hit = true;
            rightChar.hitTimer = 0.6f;
            rightChar.knockback = 0.35f;
            rightChar.hp -= 12.0f;
            if (rightChar.hp < 0.0f) rightChar.hp = 0.0f;

            if (rightChar.hp <= 0.0f) {
                rightChar.dead = true;
                rightChar.hit = false;
            }

            cameraShake = 1.0f;
            shockwaveTimer = 1.0f;
            shockwaveX = rightChar.x;
            shockwaveY = rightChar.y + 1.0f;
            shockwaveZ = rightChar.z;
        }

        if (projectile.owner == 2 && projectile.x <= leftChar.x + 1.0f) {
            projectile.active = false;
            leftChar.hit = true;
            leftChar.hitTimer = 0.6f;
            leftChar.knockback = 0.35f;
            leftChar.hp -= 12.0f;
            if (leftChar.hp < 0.0f) leftChar.hp = 0.0f;

            if (leftChar.hp <= 0.0f) {
                leftChar.dead = true;
                leftChar.hit = false;
            }

            cameraShake = 1.0f;
            shockwaveTimer = 1.0f;
            shockwaveX = leftChar.x;
            shockwaveY = leftChar.y + 1.0f;
            shockwaveZ = leftChar.z;
        }
    }

    updateFalling(leftChar);
    updateFalling(rightChar);

    if (cameraShake > 0.0f) {
        cameraShake -= dt * 2.0f;
        if (cameraShake < 0.0f) cameraShake = 0.0f;
    }

    if (shockwaveTimer > 0.0f) {
        shockwaveTimer -= dt * 1.8f;
        if (shockwaveTimer < 0.0f) shockwaveTimer = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    // Color buffer membersihkan warna layar, depth buffer membersihkan informasi kedalaman.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setProjection();
    setupLighting();
    glShadeModel(shadingMode);

    drawArena();
    drawCharacter(leftChar, 1);
    drawCharacter(rightChar, -1);
    drawProjectile();
    drawShockwave();
    drawOverlay();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
        case 'A':
            attackLeft();
            break;
        case 'l':
        case 'L':
            attackRight();
            break;
        case '1':
            // Tombol angka mengganti preset view, lalu kamera bisa digerakkan lagi dengan arrow key.
            projectionMode = PROJ_ORTHO;
            setCameraPreset(projectionMode);
            break;
        case '2':
            projectionMode = PROJ_1POINT;
            setCameraPreset(projectionMode);
            break;
        case '3':
            projectionMode = PROJ_2POINT;
            setCameraPreset(projectionMode);
            break;
        case '4':
            projectionMode = PROJ_3POINT;
            setCameraPreset(projectionMode);
            break;
        case 'f':
        case 'F':
            shadingMode = GL_FLAT;
            break;
        case 'g':
        case 'G':
            shadingMode = GL_SMOOTH;
            break;
        case 'r':
        case 'R':
            resetBattle();
            break;
        case 27:
            exit(0);
            break;
    }
}

// Handler tombol khusus GLUT untuk arrow key saat tombol ditekan.
// Nilainya disimpan sebagai arah gerak agar kamera bisa bergerak terus selama tombol ditahan.
void specialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            cameraMoveDirection = 1;
            break;
        case GLUT_KEY_DOWN:
            cameraMoveDirection = -1;
            break;
        case GLUT_KEY_LEFT:
            cameraTurnDirection = -1;
            break;
        case GLUT_KEY_RIGHT:
            cameraTurnDirection = 1;
            break;
    }
}

// Saat arrow key dilepas, arah gerak/putar dikembalikan ke nol agar kamera berhenti.
void specialKeyboardUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if (cameraMoveDirection > 0) cameraMoveDirection = 0;
            break;
        case GLUT_KEY_DOWN:
            if (cameraMoveDirection < 0) cameraMoveDirection = 0;
            break;
        case GLUT_KEY_LEFT:
            if (cameraTurnDirection < 0) cameraTurnDirection = 0;
            break;
        case GLUT_KEY_RIGHT:
            if (cameraTurnDirection > 0) cameraTurnDirection = 0;
            break;
    }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
}

void initGL() {
    glClearColor(0.78f, 0.90f, 1.00f, 1.0f);

    // Mengaktifkan depth test, lighting, light source, dan material default.
    lighting();
    glShadeModel(GL_SMOOTH);
    setCameraPreset(projectionMode);

    initCharacters();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // GLUT_DEPTH meminta depth buffer agar depth testing dapat bekerja.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("3D Monster Battle - Orthographic dan Perspective 1 2 3 Vanishing Point");

    initGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // Callback arrow key untuk movable camera.
    glutSpecialFunc(specialKeyboard);
    glutSpecialUpFunc(specialKeyboardUp);
    glutIgnoreKeyRepeat(1);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
