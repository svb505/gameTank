#include "HUD.h"
#include "projectile.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "text.h"
#include <format>
#include "tank.h"
#include <vector>
#include "variables.h"
#include "camera.h"

void drawHUD(int WW,int WH,float lastHit) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    std::string dist = std::format("Distance: {:.1f} m", lastHit);
    RenderTextHUD(WW / 2, WH / 2 - 100, 1, 1, 1, dist.c_str(), WW, WH);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}
void Draw2DAim(Tank& tank, CameraParams& cam){
    float turretRad;

    if (tank.getTurretLocked())
        turretRad = (tank.getBodyYaw() + tank.getTurretYaw()) * PI / 180.0f;
    else
        turretRad = (tank.getTurretYaw() - 90.0f) * PI / 180.0f;

    float scale = pow(cam.baseFov / cam.fov, 2.0f);

    const float size = 60.0f * scale;
    constexpr float gap = 8.0f;
    const float height = 40.0f * scale;

    constexpr float barrelLength = 4.9f;
    constexpr float barrelHeight = 0.9f;

    float pitchRad = tank.getGunPitch() * PI / 180.0f;

    svbmath::Vec3 pos = tank.getCurrentPos();

    // End of the barrel
    float startX = pos.x + sinf(turretRad) * cosf(pitchRad) * barrelLength;
    float startY = pos.y + barrelHeight - sinf(pitchRad) * barrelLength;
    float startZ = pos.z + cosf(turretRad) * cosf(pitchRad) * barrelLength;

    float dirX = startX - pos.x;
    float dirY = startY - (pos.y + barrelHeight);
    float dirZ = startZ - pos.z;


    // Normilize
    float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);

    if (len == 0.0f) return;

    dirX /= len;
    dirY /= len;
    dirZ /= len;

    constexpr float AIM_DISTANCE = 10000.0f;

    float targetX = startX + dirX * AIM_DISTANCE;
    float targetY = startY + dirY * AIM_DISTANCE;
    float targetZ = startZ + dirZ * AIM_DISTANCE;

    // Camera matrix
    GLdouble model[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble screenX;
    GLdouble screenY;
    GLdouble screenZ;

    if (!gluProject(targetX, targetY, targetZ, model, projection, viewport,
        &screenX, &screenY, &screenZ)){
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    //2D mode
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,viewport[2],0,viewport[3]);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float x = static_cast<float>(screenX);
    float y = static_cast<float>(screenY);

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);

    glBegin(GL_LINES);

    glVertex2f(x - size, y);
    glVertex2f(x - gap, y);

    glVertex2f(x + gap, y);
    glVertex2f(x + size, y);

    glVertex2f(x - gap, y);
    glVertex2f(x, y + gap);

    glVertex2f(x, y + gap);
    glVertex2f(x + gap, y);

    glVertex2f(x, y);
    glVertex2f(x, y - height);

    glEnd();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}