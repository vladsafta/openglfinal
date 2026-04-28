#define GL_SILENCE_DEPRECATION
#include <cstdlib>
#include <GLUT/glut.h>
#include <cmath>
#include <vector>
#include <algorithm>

static GLuint gGrassTex = 0;
static GLuint gSkyTex   = 0;
static GLuint gRockTex  = 0;
static GLuint gRoadTex  = 0;

static float gCamX = 9.0f;
static float gCamY = 8.0f;
static float gCamZ = 17.0f;
static float gYaw = -152.0f;
static float gPitch = -20.0f;

struct Lamp {
    float x;
    float z;
    float height;
};

static Lamp gLamps[] = {
    {-7.5f, -1.5f, 3.8f},
    {-1.5f,  4.8f, 3.8f},
    { 4.5f, -4.8f, 3.8f},
    { 7.2f,  1.8f, 3.8f}
};

static const int gLampCount = sizeof(gLamps) / sizeof(gLamps[0]);

struct BuildingDef {
    float x;
    float z;
    float sx;
    float sy;
    float sz;
};

static BuildingDef gBuildings[] = {
    {-8.0f, -7.5f, 1.4f, 3.0f, 1.4f},
    {-5.8f, -7.2f, 1.5f, 4.0f, 1.5f},
    {-3.3f, -7.0f, 1.3f, 2.6f, 1.3f},
    { 8.0f,  7.3f, 1.5f, 3.6f, 1.5f},
    { 5.6f,  7.0f, 1.7f, 4.4f, 1.7f},
    { 3.2f,  7.2f, 1.4f, 3.0f, 1.4f}
};

static const int gBuildingCount = sizeof(gBuildings) / sizeof(gBuildings[0]);

static float gCarX = 0.0f;
static float gCarZ = -7.8f;
static float gCarAngle = 0.0f;

static float gTime = 0.0f;

struct RandomMover {
    float baseX;
    float baseZ;
    float phase;
    float speed;
    float amplitudeX;
    float amplitudeZ;
    float r;
    float g;
    float b;
};

static RandomMover gRandomMovers[] = {
    {-6.8f,  1.2f, 0.0f, 0.9f, 0.8f, 0.5f, 0.95f, 0.20f, 0.20f},
    {-2.2f, -1.5f, 1.7f, 1.2f, 0.9f, 0.6f, 0.20f, 0.30f, 0.95f},
    { 5.2f,  3.0f, 3.1f, 1.0f, 0.7f, 0.8f, 0.95f, 0.80f, 0.15f}
};

static const int gRandomMoverCount = sizeof(gRandomMovers) / sizeof(gRandomMovers[0]);

float degToRad(float degrees) {
    return degrees * 3.1415926f / 180.0f;
}

void computeCameraDirection(float& dx, float& dy, float& dz) {
    float yawRad = degToRad(gYaw);
    float pitchRad = degToRad(gPitch);

    dx = std::cos(pitchRad) * std::cos(yawRad);
    dy = std::sin(pitchRad);
    dz = std::cos(pitchRad) * std::sin(yawRad);
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat globalAmbient[] = {0.40f, 0.40f, 0.40f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    glEnable(GL_LIGHT0);
    GLfloat light0Pos[] = {0.0f, 7.5f, 0.0f, 1.0f};
    GLfloat light0Diffuse[] = {0.90f, 0.90f, 0.85f, 1.0f};
    GLfloat light0Specular[] = {0.50f, 0.50f, 0.50f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);

    glEnable(GL_LIGHT1);
    GLfloat light1Pos[] = {gLamps[0].x, -5.0f + gLamps[0].height, gLamps[0].z, 1.0f};
    GLfloat light1Diffuse[] = {0.80f, 0.72f, 0.48f, 1.0f};
    GLfloat light1Specular[] = {0.60f, 0.55f, 0.35f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Specular);

    glEnable(GL_LIGHT2);
    GLfloat light2Pos[] = {gLamps[1].x, -5.0f + gLamps[1].height, gLamps[1].z, 1.0f};
    GLfloat light2Diffuse[] = {0.72f, 0.68f, 0.45f, 1.0f};
    GLfloat light2Specular[] = {0.55f, 0.50f, 0.30f, 1.0f};
    glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2Diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2Specular);
}

void drawLampPost(float x, float z, float height) {
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(x, -5.0f, z);

    glColor3f(0.20f, 0.20f, 0.22f);
    glPushMatrix();
    glTranslatef(0.0f, height * 0.5f, 0.0f);
    glScalef(0.16f, height, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.30f, 0.30f, 0.32f);
    glPushMatrix();
    glTranslatef(0.0f, height + 0.15f, 0.0f);
    glScalef(0.70f, 0.10f, 0.10f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.28f, height + 0.05f, 0.0f);
    glColor3f(1.0f, 0.95f, 0.65f);
    glutSolidSphere(0.18f, 16, 16);
    glPopMatrix();

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void drawLampPosts() {
    for (int i = 0; i < gLampCount; ++i) {
        drawLampPost(gLamps[i].x, gLamps[i].z, gLamps[i].height);
    }
}

bool intersectsSceneObjects(float x, float z, float halfWidth, float halfDepth) {
    for (int i = 0; i < gBuildingCount; ++i) {
        float buildingHalfX = gBuildings[i].sx * 0.5f + 0.25f;
        float buildingHalfZ = gBuildings[i].sz * 0.5f + 0.25f;

        if (std::fabs(x - gBuildings[i].x) < (halfWidth + buildingHalfX) &&
            std::fabs(z - gBuildings[i].z) < (halfDepth + buildingHalfZ)) {
            return true;
        }
    }

    const float reliefCx = 1.5f;
    const float reliefCz = 0.5f;
    const float reliefHalf = 2.2f;
    if (std::fabs(x - reliefCx) < (halfWidth + reliefHalf) &&
        std::fabs(z - reliefCz) < (halfDepth + reliefHalf)) {
        return true;
    }

    const float treeHalf = 0.80f;
    const float treePositions[6][2] = {
        {-8.3f,  5.0f},
        {-6.5f,  6.8f},
        {-4.5f,  5.5f},
        { 8.2f, -5.2f},
        { 6.3f, -6.7f},
        { 4.2f, -5.6f}
    };

    for (int i = 0; i < 6; ++i) {
        if (std::fabs(x - treePositions[i][0]) < (halfWidth + treeHalf) &&
            std::fabs(z - treePositions[i][1]) < (halfDepth + treeHalf)) {
            return true;
        }
    }

    const float lampHalf = 0.45f;
    for (int i = 0; i < gLampCount; ++i) {
        if (std::fabs(x - gLamps[i].x) < (halfWidth + lampHalf) &&
            std::fabs(z - gLamps[i].z) < (halfDepth + lampHalf)) {
            return true;
        }
    }

    return false;
}

void drawCar(float x, float z, float angle) {
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(x, -4.45f, z);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    glColor3f(0.85f, 0.10f, 0.10f);
    glPushMatrix();
    glScalef(1.4f, 0.45f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.90f, 0.35f, 0.20f);
    glPushMatrix();
    glTranslatef(0.0f, 0.38f, 0.0f);
    glScalef(0.8f, 0.35f, 0.75f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef( 0.45f, -0.28f,  0.38f); glutSolidTorus(0.06f, 0.13f, 12, 12); glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.45f, -0.28f,  0.38f); glutSolidTorus(0.06f, 0.13f, 12, 12); glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.45f, -0.28f, -0.38f); glutSolidTorus(0.06f, 0.13f, 12, 12); glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.45f, -0.28f, -0.38f); glutSolidTorus(0.06f, 0.13f, 12, 12); glPopMatrix();

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void drawShadowPolygon(float cx, float cz, float sx, float sz, float lampX, float lampZ, float darkness);

void drawControlledObjectShadow() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < gLampCount; ++i) {
        if (std::fabs(gCarX - gLamps[i].x) < 0.001f && std::fabs(gCarZ - gLamps[i].z) < 0.001f) {
            continue;
        }
        drawShadowPolygon(gCarX, gCarZ, 0.55f, 0.35f, gLamps[i].x, gLamps[i].z, 0.24f);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void drawRuleBasedMover() {
    glDisable(GL_TEXTURE_2D);

    float t = gTime * 0.7f;
    float x = 5.9f * std::cos(t);
    float z = 3.3f * std::sin(t);
    float angle = -t * 180.0f / 3.1415926f;

    glPushMatrix();
    glTranslatef(x, -4.55f, z);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    glColor3f(0.15f, 0.25f, 0.95f);
    glPushMatrix();
    glScalef(0.95f, 0.35f, 0.55f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.30f, 0.55f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    glScalef(0.50f, 0.22f, 0.50f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef( 0.30f, -0.18f,  0.23f); glutSolidTorus(0.04f, 0.09f, 10, 10); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.30f, -0.18f,  0.23f); glutSolidTorus(0.04f, 0.09f, 10, 10); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.30f, -0.18f, -0.23f); glutSolidTorus(0.04f, 0.09f, 10, 10); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.30f, -0.18f, -0.23f); glutSolidTorus(0.04f, 0.09f, 10, 10); glPopMatrix();

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void drawRandomMovers() {
    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < gRandomMoverCount; ++i) {
        float t = gTime * gRandomMovers[i].speed + gRandomMovers[i].phase;
        float x = gRandomMovers[i].baseX
            + gRandomMovers[i].amplitudeX * std::sin(t)
            + 0.25f * std::cos(1.9f * t + 0.7f);
        float z = gRandomMovers[i].baseZ
            + gRandomMovers[i].amplitudeZ * std::cos(1.3f * t)
            + 0.20f * std::sin(2.1f * t + 0.4f);
        float y = -4.35f + 0.15f * std::sin(2.5f * t);

        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(gRandomMovers[i].r, gRandomMovers[i].g, gRandomMovers[i].b);
        glutSolidSphere(0.28f, 16, 16);

        glTranslatef(0.0f, 0.40f, 0.0f);
        glColor3f(gRandomMovers[i].r * 0.8f, gRandomMovers[i].g * 0.8f, gRandomMovers[i].b * 0.8f);
        glutSolidSphere(0.16f, 14, 14);
        glPopMatrix();
    }

    glEnable(GL_TEXTURE_2D);
}

void timer(int value) {
    gTime += 0.03f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void drawShadowPolygon(float cx, float cz, float sx, float sz, float lampX, float lampZ, float darkness) {
    float dx = cx - lampX;
    float dz = cz - lampZ;
    float len = std::sqrt(dx * dx + dz * dz);
    if (len < 0.001f) len = 0.001f;
    dx /= len;
    dz /= len;

    float px = -dz;
    float pz = dx;

    float nearLen = 0.7f;
    float farLen = 2.6f;

    float nx1 = cx + px * sx * 0.7f;
    float nz1 = cz + pz * sz * 0.7f;
    float nx2 = cx - px * sx * 0.7f;
    float nz2 = cz - pz * sz * 0.7f;

    float fx1 = cx + dx * farLen + px * sx;
    float fz1 = cz + dz * farLen + pz * sz;
    float fx2 = cx + dx * farLen - px * sx;
    float fz2 = cz + dz * farLen - pz * sz;

    glColor4f(0.0f, 0.0f, 0.0f, darkness);
    glBegin(GL_QUADS);
        glVertex3f(nx1 + dx * nearLen, -4.96f, nz1 + dz * nearLen);
        glVertex3f(nx2 + dx * nearLen, -4.96f, nz2 + dz * nearLen);
        glVertex3f(fx2, -4.96f, fz2);
        glVertex3f(fx1, -4.96f, fz1);
    glEnd();
}

void drawProjectedShadows() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < gLampCount; ++i) {
        float lampX = gLamps[i].x;
        float lampZ = gLamps[i].z;

        drawShadowPolygon(-8.0f, -7.5f, 0.8f, 0.8f, lampX, lampZ, 0.16f);
        drawShadowPolygon(-5.8f, -7.2f, 0.85f, 0.85f, lampX, lampZ, 0.16f);
        drawShadowPolygon(-3.3f, -7.0f, 0.75f, 0.75f, lampX, lampZ, 0.16f);
        drawShadowPolygon( 8.0f,  7.3f, 0.85f, 0.85f, lampX, lampZ, 0.16f);
        drawShadowPolygon( 5.6f,  7.0f, 0.95f, 0.95f, lampX, lampZ, 0.16f);
        drawShadowPolygon( 3.2f,  7.2f, 0.80f, 0.80f, lampX, lampZ, 0.16f);

        drawShadowPolygon(-8.3f,  5.0f, 0.55f, 0.55f, lampX, lampZ, 0.12f);
        drawShadowPolygon(-6.5f,  6.8f, 0.55f, 0.55f, lampX, lampZ, 0.12f);
        drawShadowPolygon(-4.5f,  5.5f, 0.55f, 0.55f, lampX, lampZ, 0.12f);
        drawShadowPolygon( 8.2f, -5.2f, 0.55f, 0.55f, lampX, lampZ, 0.12f);
        drawShadowPolygon( 6.3f, -6.7f, 0.55f, 0.55f, lampX, lampZ, 0.12f);
        drawShadowPolygon( 4.2f, -5.6f, 0.55f, 0.55f, lampX, lampZ, 0.12f);
        if (i != 0) drawShadowPolygon(-7.5f, -1.5f, 0.16f, 0.16f, lampX, lampZ, 0.22f);
        if (i != 1) drawShadowPolygon(-1.5f,  4.8f, 0.16f, 0.16f, lampX, lampZ, 0.22f);
        if (i != 2) drawShadowPolygon( 4.5f, -4.8f, 0.16f, 0.16f, lampX, lampZ, 0.22f);
        if (i != 3) drawShadowPolygon( 7.2f,  1.8f, 0.16f, 0.16f, lampX, lampZ, 0.22f);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void keyboard(unsigned char key, int, int) {
    float dx, dy, dz;
    computeCameraDirection(dx, dy, dz);

    float rightX = -dz;
    float rightZ = dx;
    float moveStep = 0.6f;
    float verticalStep = 0.45f;

    switch (key) {
        case 'w': case 'W':
            gCamX += dx * moveStep;
            gCamY += dy * moveStep;
            gCamZ += dz * moveStep;
            break;
        case 's': case 'S':
            gCamX -= dx * moveStep;
            gCamY -= dy * moveStep;
            gCamZ -= dz * moveStep;
            break;
        case 'a': case 'A':
            gCamX -= rightX * moveStep;
            gCamZ -= rightZ * moveStep;
            break;
        case 'd': case 'D':
            gCamX += rightX * moveStep;
            gCamZ += rightZ * moveStep;
            break;
        case 'q': case 'Q':
            gCamY += verticalStep;
            break;
        case 'e': case 'E':
            gCamY -= verticalStep;
            break;
        case 27:
            std::exit(0);
            break;
        default:
            break;
    }

    float carStep = 0.45f;
    float carHalfWidth = 0.70f;
    float carHalfDepth = 0.45f;
    float carRad = degToRad(gCarAngle);

    switch (key) {
        case 'i': case 'I': {
            float nextX = gCarX + std::cos(carRad) * carStep;
            float nextZ = gCarZ + std::sin(carRad) * carStep;
            if (!intersectsSceneObjects(nextX, nextZ, carHalfWidth, carHalfDepth) &&
                nextX > -9.2f && nextX < 9.2f && nextZ > -9.2f && nextZ < 9.2f) {
                gCarX = nextX;
                gCarZ = nextZ;
            }
            break;
        }
        case 'k': case 'K': {
            float nextX = gCarX - std::cos(carRad) * carStep;
            float nextZ = gCarZ - std::sin(carRad) * carStep;
            if (!intersectsSceneObjects(nextX, nextZ, carHalfWidth, carHalfDepth) &&
                nextX > -9.2f && nextX < 9.2f && nextZ > -9.2f && nextZ < 9.2f) {
                gCarX = nextX;
                gCarZ = nextZ;
            }
            break;
        }
        case 'j': case 'J':
            gCarAngle += 6.0f;
            break;
        case 'l': case 'L':
            gCarAngle -= 6.0f;
            break;
        default:
            break;
    }

    gCamX = std::max(-18.0f, std::min(18.0f, gCamX));
    gCamY = std::max(-2.0f, std::min(18.0f, gCamY));
    gCamZ = std::max(-18.0f, std::min(18.0f, gCamZ));
    glutPostRedisplay();
}

void specialKeys(int key, int, int) {
    float rotStep = 4.0f;

    switch (key) {
        case GLUT_KEY_LEFT:
            gYaw -= rotStep;
            break;
        case GLUT_KEY_RIGHT:
            gYaw += rotStep;
            break;
        case GLUT_KEY_UP:
            gPitch += rotStep;
            break;
        case GLUT_KEY_DOWN:
            gPitch -= rotStep;
            break;
        default:
            break;
    }

    gPitch = std::max(-80.0f, std::min(80.0f, gPitch));
    glutPostRedisplay();
}

void setupProjection(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (double)w / (double)h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void createGrassTexture() {
    const int w = 256, h = 256;
    std::vector<unsigned char> data(w * h * 3);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = (y * w + x) * 3;

            float a = 0.5f + 0.5f * std::sin(x * 0.18f) * std::cos(y * 0.22f);
            float b = 0.5f + 0.5f * std::sin((x + y) * 0.09f);

            data[i + 0] = (unsigned char)(20 + a * 25);
            data[i + 1] = (unsigned char)(110 + a * 90);
            data[i + 2] = (unsigned char)(20 + b * 20);
        }
    }

    glGenTextures(1, &gGrassTex);
    glBindTexture(GL_TEXTURE_2D, gGrassTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}

void createSkyTexture() {
    const int w = 512, h = 256;
    std::vector<unsigned char> data(w * h * 3);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float t = (float)y / (float)(h - 1);

            unsigned char r = (unsigned char)((1.0f - t) * 175 + t * 110);
            unsigned char g = (unsigned char)((1.0f - t) * 225 + t * 190);
            unsigned char b = (unsigned char)((1.0f - t) * 255 + t * 245);

            float xf = (float)x / (float)(w - 1);
            float cloud = 0.5f + 0.5f * std::sin(10.0f * xf) * std::cos(15.0f * t);

            if (t < 0.35f && cloud > 0.80f) {
                r = 245; g = 245; b = 245;
            }

            int i = (y * w + x) * 3;
            data[i + 0] = r;
            data[i + 1] = g;
            data[i + 2] = b;
        }
    }

    glGenTextures(1, &gSkyTex);
    glBindTexture(GL_TEXTURE_2D, gSkyTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}

void createRockTexture() {
    const int w = 256, h = 256;
    std::vector<unsigned char> data(w * h * 3);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = (y * w + x) * 3;

            float a = 0.5f + 0.5f * std::sin(x * 0.24f) * std::cos(y * 0.19f);
            float b = 0.5f + 0.5f * std::sin((x + y) * 0.11f);
            float mix = 0.65f * a + 0.35f * b;

            unsigned char c = (unsigned char)(120 + mix * 100);
            data[i + 0] = c;
            data[i + 1] = (unsigned char)(c - 8);
            data[i + 2] = (unsigned char)(c - 16);
        }
    }

    glGenTextures(1, &gRockTex);
    glBindTexture(GL_TEXTURE_2D, gRockTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}

void createRoadTexture() {
    const int w = 256, h = 256;
    std::vector<unsigned char> data(w * h * 3);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = (y * w + x) * 3;

            unsigned char r = 65;
            unsigned char g = 65;
            unsigned char b = 70;

            float noise = 0.5f + 0.5f * std::sin(x * 0.31f) * std::cos(y * 0.27f);
            r = (unsigned char)(r + noise * 18);
            g = (unsigned char)(g + noise * 18);
            b = (unsigned char)(b + noise * 18);

            
            if (x > 118 && x < 138 && ((y / 20) % 2 == 0)) {
                r = 235; g = 235; b = 110;
            }

            data[i + 0] = r;
            data[i + 1] = g;
            data[i + 2] = b;
        }
    }

    glGenTextures(1, &gRoadTex);
    glBindTexture(GL_TEXTURE_2D, gRoadTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}

void initScene() {
    glClearColor(0.78f, 0.90f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    createGrassTexture();
    createSkyTexture();
    createRockTexture();
    createRoadTexture();
}

void drawCubeBounds(float halfSize) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);
    glutWireCube(halfSize * 2.0f);
    glEnable(GL_TEXTURE_2D);
}

void drawGround(float halfSize) {
    glBindTexture(GL_TEXTURE_2D, gGrassTex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -5.0f, -halfSize);
        glTexCoord2f(6.0f, 0.0f); glVertex3f( halfSize, -5.0f, -halfSize);
        glTexCoord2f(6.0f, 6.0f); glVertex3f( halfSize, -5.0f,  halfSize);
        glTexCoord2f(0.0f, 6.0f); glVertex3f(-halfSize, -5.0f,  halfSize);
    glEnd();
}

void drawSkyWalls(float halfSize) {
    glBindTexture(GL_TEXTURE_2D, gSkyTex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 0.0f, 1.0f);

    const float yBottom = -5.0f;
    const float yTop    =  3.0f;
    const float zBack   = -9.8f;
    const float xLeft   = -9.8f;
    const float xRight  =  9.8f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, yBottom, zBack);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize, yBottom, zBack);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize, yTop,    zBack);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, yTop,    zBack);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(xLeft, yBottom,  halfSize);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xLeft, yBottom, -halfSize);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xLeft, yTop,    -halfSize);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xLeft, yTop,     halfSize);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(xRight, yBottom, -halfSize);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xRight, yBottom,  halfSize);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xRight, yTop,     halfSize);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xRight, yTop,    -halfSize);
    glEnd();
}

void drawRelief() {
    glBindTexture(GL_TEXTURE_2D, gRockTex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);

    const float baseY  = -5.0f;
    const float size   = 2.0f;
    const float height = 4.5f;
    const float cx     = 1.5f;
    const float cz     = 0.5f;

    glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(cx - size, baseY, cz - size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cx + size, baseY, cz - size);
        glTexCoord2f(0.5f, 0.0f); glVertex3f(cx, baseY + height, cz);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(cx + size, baseY, cz - size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cx + size, baseY, cz + size);
        glTexCoord2f(0.5f, 0.0f); glVertex3f(cx, baseY + height, cz);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(cx + size, baseY, cz + size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cx - size, baseY, cz + size);
        glTexCoord2f(0.5f, 0.0f); glVertex3f(cx, baseY + height, cz);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(cx - size, baseY, cz + size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(cx - size, baseY, cz - size);
        glTexCoord2f(0.5f, 0.0f); glVertex3f(cx, baseY + height, cz);
    glEnd();
}

void drawRoad() {
    glBindTexture(GL_TEXTURE_2D, gRoadTex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);

    const float y = -4.98f;
    const int segments = 160;
    const float outerA = 7.0f;
    const float outerB = 4.2f;
    const float innerA = 4.8f;
    const float innerB = 2.4f;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float t = 2.0f * 3.1415926f * (float)i / (float)segments;
        float ct = std::cos(t);
        float st = std::sin(t);

        float xOuter = outerA * ct;
        float zOuter = outerB * st;
        float xInner = innerA * ct;
        float zInner = innerB * st;

        float u = 8.0f * (float)i / (float)segments;

        glTexCoord2f(u, 0.0f); glVertex3f(xOuter, y, zOuter);
        glTexCoord2f(u, 1.0f); glVertex3f(xInner, y, zInner);
    }
    glEnd();
}

void drawBuilding(float x, float z, float sx, float sy, float sz) {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(x, -5.0f + sy / 2.0f, z);
    glScalef(sx, sy, sz);
    glNormal3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.75f, 0.55f, 0.40f);
    glutSolidCube(1.0f);

    glTranslatef(0.0f, 0.55f, 0.0f);
    glScalef(1.05f, 0.25f, 1.05f);
    glColor3f(0.55f, 0.15f, 0.15f);
    glutSolidCube(1.0f);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void drawTree(float x, float z) {
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(x, -4.2f, z);
    glNormal3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.45f, 0.25f, 0.10f);
    glPushMatrix();
    glScalef(0.35f, 1.6f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.10f, 0.55f, 0.15f);
    glTranslatef(0.0f, 1.2f, 0.0f);
    glutSolidSphere(0.85f, 16, 16);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void drawStaticObjects() {
    for (int i = 0; i < gBuildingCount; ++i) {
        drawBuilding(gBuildings[i].x, gBuildings[i].z, gBuildings[i].sx, gBuildings[i].sy, gBuildings[i].sz);
    }

    // 6 pomi
    drawTree(-8.3f,  5.0f);
    drawTree(-6.5f,  6.8f);
    drawTree(-4.5f,  5.5f);

    drawTree( 8.2f, -5.2f);
    drawTree( 6.3f, -6.7f);
    drawTree( 4.2f, -5.6f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float dx, dy, dz;
    computeCameraDirection(dx, dy, dz);
    gluLookAt(gCamX, gCamY, gCamZ,
              gCamX + dx, gCamY + dy, gCamZ + dz,
              0.0f, 1.0f, 0.0f);

    setupLighting();

    drawGround(10.0f);
    drawSkyWalls(10.0f);

    // P1
    drawRelief();

    // P2
    drawRoad();
    drawStaticObjects();

    // C1
    drawCar(gCarX, gCarZ, gCarAngle);
    drawControlledObjectShadow();

    // C2
    drawRuleBasedMover();
    drawRandomMovers();

    // P3
    drawLampPosts();
    drawProjectedShadows();

    glDisable(GL_LIGHTING);
    drawCubeBounds(10.0f);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    setupProjection(w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("P1 + P2 - Scena in cub");

    initScene();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}