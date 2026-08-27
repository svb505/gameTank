#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "svbmath.h" 
#include "badges.h"
#include "enemyes.h"
#include "camera.h"
#include <cmath>

void DrawSpawnMarker2D(const svbmath::Vec3& pos, bool selected, float size, float r, float g, float b) {
    glPushMatrix();

    glTranslatef(pos.x, pos.y + 0.05f, pos.z);

    if (!selected) glColor3f(r, g, b);
    else glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_LINES);
    glVertex3f(-size * 0.5f, 0.0f, -size * 0.5f);
    glVertex3f(size * 0.5f, 0.0f, size * 0.5f);

    glVertex3f(-size * 0.5f, 0.0f, size * 0.5f);
    glVertex3f(size * 0.5f, 0.0f, -size * 0.5f);
    glEnd();

    glPopMatrix();
}
void drawRadar(const svbmath::Vec2& pos, float size, float angle) {
    // circle
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 40; i++) {
        float a = i * 2.0f * 3.14159f / 40;
        glVertex2f(pos.x + cos(a) * size,pos.y + sin(a) * size);
    }
    glEnd();

    // ray
    glBegin(GL_LINES);
    glVertex2f(pos.x, pos.y);
    glVertex2f(pos.x + cos(angle) * size,pos.y + sin(angle) * size);
    glEnd();
}
void drawTank(const svbmath::Vec2& pos, float size, float angle) {
    // body
    glColor3f(0.3f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(pos.x - size, pos.y - size * 0.5f);
    glVertex2f(pos.x + size, pos.y - size * 0.5f);
    glVertex2f(pos.x + size, pos.y + size * 0.5f);
    glVertex2f(pos.x - size, pos.y + size * 0.5f);
    glEnd();

    // turret
    glColor3f(0.2f, 0.4f, 0.2f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(pos.x, pos.y);
    for (int i = 0; i <= 20; i++) {
        float a = i * 2.0f * 3.14159f / 20;
        glVertex2f(pos.x + cos(a) * size * 0.5f,pos.y + sin(a) * size * 0.5f);
    }
    glEnd();

    // gun
    glBegin(GL_LINES);
    glVertex2f(pos.x, pos.y);
    glVertex2f(pos.x + cos(angle) * size * 1.5f,pos.y + sin(angle) * size * 1.5f);
    glEnd();
}
void drawHouse(const svbmath::Vec2& pos, float size) {
    // wall
    glColor3f(0.7f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(pos.x - size, pos.y);
    glVertex2f(pos.x + size, pos.y);
    glVertex2f(pos.x + size, pos.y + size);
    glVertex2f(pos.x - size, pos.y + size);
    glEnd();

    // roof
    glColor3f(0.6f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(pos.x - size, pos.y + size);
    glVertex2f(pos.x + size, pos.y + size);
    glVertex2f(pos.x, pos.y + size * 1.8f);
    glEnd();
}
void drawPlayerIcon(const svbmath::Vec2& pos, float size, float angle) {
    glColor3f(1.0f, 1.0f, 0.0f); 

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0.0f);
    glRotatef(angle * 180.0f / 3.14159f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, size);        
    glVertex2f(-size * 0.5f, -size); 
    glVertex2f(size * 0.5f, -size); 
    glEnd();

    glPopMatrix();
}
void Begin2D(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
}
void End2D() {
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
void WorldToScreen(float worldX, float worldZ, float centerX, float centerZ, float yaw,
    const Camera& cam,float scale, float screenW, float screenH, float& outX, float& outY){

    float dx = worldX - centerX;
    float dz = worldZ - centerZ;

    outX = dx * scale + screenW * 0.5f;
    outY = screenH * 0.5f - dz * scale;
}
void RenderBadges(int ww, int wh, float scale, Tank& tank,Camera& cam) {

    Begin2D(ww, wh); 

    for (auto e : entities) {
        if (healths[e].destroyed) continue;

        auto& t = transforms[e];
        auto& r = renders[e];

        float sx, sy;

        WorldToScreen(t.pos.x, t.pos.z, tank.getCurrentPos().x, tank.getCurrentPos().z, 
            cam.getCamParams().cameraYaw,cam,scale, ww, wh,sx, sy);

        switch (r.type) {
        case RenderType::Tank:
            drawTank({ sx, sy }, 7, t.angle - cam.getCamParams().cameraYaw);
            break;

        case RenderType::Radar:
            drawRadar({ sx, sy }, 8, t.angle - cam.getCamParams().cameraYaw);
            break;

        case RenderType::Apartment:
            drawHouse({ sx, sy }, 6);
            break;
        }
    }

    End2D();
}