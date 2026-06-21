#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "effects.h"
#include "text.h"
#include "tank.h"
#include "enemyes.h"
#include "bounds.h"
#include "svbmath.h"
#include "projectileSystem.h"
#include "sounds.h"
#include "Logger.h"
#include "texture.h"

bool showBars = true;

std::map<RenderType, std::string> rendersMap = {{RenderType::Tank,"Tank"}, 
    {RenderType::Radar,"Radar"}, {RenderType::Apartment,"Apartment"} };

std::vector<Entity> entities;

std::unordered_map<Entity, Transform> transforms;
std::unordered_map<Entity, Health> healths;
std::unordered_map<Entity, RenderComponent> renders;
std::unordered_map<Entity, TankComponent> tanks;
std::unordered_map<Entity, RadarComponent> radars;
std::unordered_map<Entity, ApartmentComponent> apartments;
std::unordered_map<Entity, Bounds> bounds;

Entity nextEntity = 1;

Entity CreateEntity() {
    Entity e = nextEntity++;
    entities.push_back(e);
    return e;
}
void DrawCube(float w, float h, float d) { 
    glBegin(GL_QUADS); 
    
    glVertex3f(-w, -h, d); 
    glVertex3f(w, -h, d); 
    glVertex3f(w, h, d); 
    glVertex3f(-w, h, d); 
    
    glVertex3f(-w, -h, -d); 
    glVertex3f(-w, h, -d); 
    glVertex3f(w, h, -d); 
    glVertex3f(w, -h, -d); 
    
    glVertex3f(-w, -h, -d); 
    glVertex3f(-w, -h, d); 
    glVertex3f(-w, h, d); 
    glVertex3f(-w, h, -d); 
    
    glVertex3f(w, -h, -d); 
    glVertex3f(w, h, -d); 
    glVertex3f(w, h, d); 
    glVertex3f(w, -h, d); 
    
    glVertex3f(-w, h, -d); 
    glVertex3f(-w, h, d); 
    glVertex3f(w, h, d); 
    glVertex3f(w, h, -d); 
    
    glVertex3f(-w, -h, -d);
    glVertex3f(w, -h, -d); 
    glVertex3f(w, -h, d); 
    glVertex3f(-w, -h, d); 
    
    glEnd(); 
}
void drawSootEffect(float x, float y, float z, int segments, float radius) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); 

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        float px = radius * cosf(angle);
        float pz = radius * sinf(angle);

        glVertex3f(px, 0.0f, pz);
    }

    glEnd();
    glPopMatrix();
}
void drawDestroyedTank(TankComponent& tank, float bodyH) {
    glPushMatrix();

    glRotatef(-10, 0, 0, 1);
    glRotatef(5, 1, 0, 0);

    glColor3f(0.1f, 0.1f, 0.1f);

    // ===== BODY =====
    glBegin(GL_QUADS);

    // Front
    glVertex3f(-1, -bodyH, 1);
    glVertex3f(1, -bodyH, 1);
    glVertex3f(1, bodyH, 1);
    glVertex3f(-1, bodyH, 1);

    // Back
    glVertex3f(-1, -bodyH, -1);
    glVertex3f(-1, bodyH, -1);
    glVertex3f(1, bodyH, -1);
    glVertex3f(1, -bodyH, -1);

    // Left
    glVertex3f(-1, -bodyH, -1);
    glVertex3f(-1, -bodyH, 1);
    glVertex3f(-1, bodyH, 1);
    glVertex3f(-1, bodyH, -1);

    // Right
    glVertex3f(1, -bodyH, -1);
    glVertex3f(1, bodyH, -1);
    glVertex3f(1, bodyH, 1);
    glVertex3f(1, -bodyH, 1);

    // Top
    glVertex3f(-1, bodyH, -1);
    glVertex3f(-1, bodyH, 1);
    glVertex3f(1, bodyH, 1);
    glVertex3f(1, bodyH, -1);

    // Bottom
    glVertex3f(-1, -bodyH, -1);
    glVertex3f(1, -bodyH, -1);
    glVertex3f(1, -bodyH, 1);
    glVertex3f(-1, -bodyH, 1);

    glEnd();

    // ===== TURRET =====
    glPushMatrix();
    glTranslatef(0.0f, bodyH + 0.4f, 0.0f);

    glRotatef(140.0f, 0, 1, 0);

    float t = 0.5f;
    glColor3f(0.15f, 0.15f, 0.15f);

    glBegin(GL_QUADS);
    glVertex3f(-t, -t, t); glVertex3f(t, -t, t); glVertex3f(t, t, t); glVertex3f(-t, t, t);
    glVertex3f(-t, -t, -t); glVertex3f(-t, t, -t); glVertex3f(t, t, -t); glVertex3f(t, -t, -t);
    glVertex3f(-t, -t, -t); glVertex3f(-t, -t, t); glVertex3f(-t, t, t); glVertex3f(-t, t, -t);
    glVertex3f(t, -t, -t); glVertex3f(t, t, -t); glVertex3f(t, t, t); glVertex3f(t, -t, t);
    glVertex3f(-t, t, -t); glVertex3f(-t, t, t); glVertex3f(t, t, t); glVertex3f(t, t, -t);
    glVertex3f(-t, -t, -t); glVertex3f(t, -t, -t); glVertex3f(t, -t, t); glVertex3f(-t, -t, t);
    glEnd();

    // ===== GUN =====
    glPushMatrix();

    glTranslatef(0.0f, 0.0f, t);

    glRotatef(-35.0f, 1, 0, 0);

    float w = 0.12f;
    float h = 0.12f;
    float len = 1.8f;

    glColor3f(0.05f, 0.05f, 0.05f);

    glBegin(GL_QUADS);

    // Front
    glVertex3f(-w, -h, len);
    glVertex3f(w, -h, len);
    glVertex3f(w, h, len);
    glVertex3f(-w, h, len);

    glVertex3f(-w, h, 0); glVertex3f(-w, h, len); glVertex3f(w, h, len); glVertex3f(w, h, 0);
    glVertex3f(-w, -h, 0); glVertex3f(w, -h, 0); glVertex3f(w, -h, len); glVertex3f(-w, -h, len);
    glVertex3f(-w, -h, 0); glVertex3f(-w, -h, len); glVertex3f(-w, h, len); glVertex3f(-w, h, 0);
    glVertex3f(w, -h, 0); glVertex3f(w, h, 0); glVertex3f(w, h, len); glVertex3f(w, -h, len);

    glEnd();

    glPopMatrix(); // gun
    glPopMatrix(); // turret
    glPopMatrix(); // tank
}
void drawTank(TankComponent& tank, float bodyH) {
    glPushMatrix();
    glColor3f(0, 0.8f, 0);

    glBegin(GL_QUADS);
    // Front
    glVertex3f(-1, -bodyH, 1);
    glVertex3f(1, -bodyH, 1);
    glVertex3f(1, bodyH, 1);
    glVertex3f(-1, bodyH, 1);

    // Back
    glVertex3f(-1, -bodyH, -1);
    glVertex3f(-1, bodyH, -1);
    glVertex3f(1, bodyH, -1);
    glVertex3f(1, -bodyH, -1);

    // Left
    glVertex3f(-1, -bodyH, -1);
    glVertex3f(-1, -bodyH, 1);
    glVertex3f(-1, bodyH, 1);
    glVertex3f(-1, bodyH, -1);

    // Right
    glVertex3f(1, -bodyH, -1);
    glVertex3f(1, bodyH, -1);
    glVertex3f(1, bodyH, 1);
    glVertex3f(1, -bodyH, 1);

    // Top
    glVertex3f(-1, bodyH, -1);
    glVertex3f(-1, bodyH, 1);
    glVertex3f(1, bodyH, 1);
    glVertex3f(1, bodyH, -1);

    // Bottom
    glVertex3f(-1, -bodyH, -1);
    glVertex3f(1, -bodyH, -1);
    glVertex3f(1, -bodyH, 1);
    glVertex3f(-1, -bodyH, 1);

    glEnd();

    // ================= TURRET =================
    glPushMatrix();
    glTranslatef(0.0f, bodyH + 0.4f, 0.0f);
    glRotatef(tank.turretAngle * 180.0f / PI + 180.0f, 0, 1, 0);

    float t = 0.5f;
    glColor3f(0, 0.6f, 0.2f);

    glBegin(GL_QUADS);
    // Front
    glVertex3f(-t, -t, t);
    glVertex3f(t, -t, t);
    glVertex3f(t, t, t);
    glVertex3f(-t, t, t);

    // Back
    glVertex3f(-t, -t, -t);
    glVertex3f(-t, t, -t);
    glVertex3f(t, t, -t);
    glVertex3f(t, -t, -t);

    // Left
    glVertex3f(-t, -t, -t);
    glVertex3f(-t, -t, t);
    glVertex3f(-t, t, t);
    glVertex3f(-t, t, -t);

    // Right
    glVertex3f(t, -t, -t);
    glVertex3f(t, t, -t);
    glVertex3f(t, t, t);
    glVertex3f(t, -t, t);

    // Top
    glVertex3f(-t, t, -t);
    glVertex3f(-t, t, t);
    glVertex3f(t, t, t);
    glVertex3f(t, t, -t);

    // Bottom
    glVertex3f(-t, -t, -t);
    glVertex3f(t, -t, -t);
    glVertex3f(t, -t, t);
    glVertex3f(-t, -t, t);

    glEnd();

    // ================= GUN =================
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, t);
    glRotatef(tank.gunAngle, 1, 0, 0);

    float w = 0.12f;
    float h = 0.12f;
    float len = 1.8f;
    glColor3f(0.2f, 0.2f, 0.2f);

    glBegin(GL_QUADS);
    // Front
    glVertex3f(-w, -h, len);
    glVertex3f(w, -h, len);
    glVertex3f(w, h, len);
    glVertex3f(-w, h, len);

    // Top
    glVertex3f(-w, h, 0);
    glVertex3f(-w, h, len);
    glVertex3f(w, h, len);
    glVertex3f(w, h, 0);

    // Bottom
    glVertex3f(-w, -h, 0);
    glVertex3f(w, -h, 0);
    glVertex3f(w, -h, len);
    glVertex3f(-w, -h, len);

    // Left
    glVertex3f(-w, -h, 0);
    glVertex3f(-w, -h, len);
    glVertex3f(-w, h, len);
    glVertex3f(-w, h, 0);

    // Right
    glVertex3f(w, -h, 0);
    glVertex3f(w, h, 0);
    glVertex3f(w, h, len);
    glVertex3f(w, -h, len);

    glEnd();

    glPopMatrix(); // gun
    glPopMatrix(); // turret
    glPopMatrix(); // tank
}
void RadarSystem(float dt) {
    for (auto& [entity, radar] : radars) {
        transforms[entity].angle += radar.rotationSpeed * dt;
        if (transforms[entity].angle >= 360.0f) transforms[entity].angle -= 360.0f;
    }
}
void drawDestroyedAppartament(ApartmentComponent& ap, float totalH){
    GLuint wallTexture = allTextures["betonD"];
    GLuint windowTexture = allTextures["windowDestroyed"];
    GLuint doorTexture = allTextures["door"];

    float halfW = ap.width;
    float halfD = ap.depth;

    float leftTop = totalH;
    float rightTop = totalH * 0.5f;

    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // ================= FRONT LEFT =================
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-halfW, 0, halfD);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.0f, 0, halfD);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.0f, leftTop, halfD);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-halfW, leftTop, halfD);

    glEnd();


    // ================= FRONT RIGHT =================
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0f, 0, halfD);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(halfW, 0, halfD);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(halfW, rightTop, halfD);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, rightTop, halfD);

    glEnd();


    // ================= BACK =================
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-halfW, 0, -halfD);

    glTexCoord2f(2.0f, 0.0f);
    glVertex3f(halfW, 0, -halfD);

    glTexCoord2f(2.0f, 2.0f);
    glVertex3f(halfW, rightTop, -halfD);

    glTexCoord2f(0.0f, 2.0f);
    glVertex3f(-halfW, leftTop, -halfD);

    glEnd();


    // ================= LEFT WALL =================
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-halfW, 0, -halfD);

    glTexCoord2f(2.0f, 0.0f);
    glVertex3f(-halfW, 0, halfD);

    glTexCoord2f(2.0f, 2.0f);
    glVertex3f(-halfW, leftTop, halfD);

    glTexCoord2f(0.0f, 2.0f);
    glVertex3f(-halfW, leftTop, -halfD);

    glEnd();


    // ================= RIGHT WALL =================
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(halfW, 0, -halfD);

    glTexCoord2f(2.0f, 0.0f);
    glVertex3f(halfW, 0, halfD);

    glTexCoord2f(2.0f, 2.0f);
    glVertex3f(halfW, rightTop, halfD);

    glTexCoord2f(0.0f, 2.0f);
    glVertex3f(halfW, rightTop, -halfD);

    glEnd();


    // ================= BROKEN TOP =================
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    glBegin(GL_TRIANGLES);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-halfW, leftTop, halfD);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-halfW, leftTop, -halfD);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(halfW, rightTop, -halfD);


    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-halfW, leftTop, halfD);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(halfW, rightTop, -halfD);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(halfW, rightTop, halfD);

    glEnd();

    glDisable(GL_TEXTURE_2D);


    // ================= WINDOWS =================
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, windowTexture);
    glColor3f(1, 1, 1);

    for (int f = 0; f < ap.floors; f++)
    {
        float y = f * ap.floorHeight + 0.2f;
        if (y > rightTop) break;

        for (int i = -2; i <= 0; i++)
        {
            float x = i * 0.7f;

            glBegin(GL_QUADS);

            glTexCoord2f(0, 0);
            glVertex3f(x - 0.2f, y, halfD + 0.01f);

            glTexCoord2f(1, 0);
            glVertex3f(x + 0.2f, y, halfD + 0.01f);

            glTexCoord2f(1, 1);
            glVertex3f(x + 0.2f, y + 0.3f, halfD + 0.01f);

            glTexCoord2f(0, 1);
            glVertex3f(x - 0.2f, y + 0.3f, halfD + 0.01f);

            glEnd();
        }
    }

    glDisable(GL_TEXTURE_2D);


    // ================= CROOKED DOOR =================
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorTexture);
    glColor3f(1, 1, 1);

    glPushMatrix();
    glTranslatef(-0.5f, 0, halfD + 0.02f);
    glRotatef(-10, 0, 0, 1);

    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);

    glTexCoord2f(1, 0);
    glVertex3f(1.0f, 0, 0);

    glTexCoord2f(1, 1);
    glVertex3f(1.0f, 1.0f, 0);

    glTexCoord2f(0, 1);
    glVertex3f(0, 1.0f, 0);

    glEnd();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);


    // ================= RUBBLE =================
    glColor3f(0.4f, 0.4f, 0.4f);

    for (int i = 0; i < 15; i++)
    {
        float x = -halfW + (i % 5) * 0.4f;
        float z = halfD + (i / 5) * 0.3f;

        glPushMatrix();
        glTranslatef(x, 0.05f, z);
        DrawCube(0.15f, 0.15f, 0.15f);
        glPopMatrix();
    }

    glPopMatrix();
}
void drawAppartament(ApartmentComponent& ap, float totalH) {
    if (ap.LOD == 1) {
        glPushMatrix();

        GLuint betonTexture = allTextures["beton"];
        GLuint windowTexture = allTextures["window"];
        GLuint doorTexture = allTextures["door"];

        // ================= BODY =================
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, betonTexture);

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);

        // ---------- FRONT ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, 0, ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, 0, ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(ap.width, totalH, ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-ap.width, totalH, ap.depth);


        // ---------- BACK ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, 0, -ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-ap.width, totalH, -ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(ap.width, totalH, -ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, 0, -ap.depth);


        // ---------- LEFT ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, 0, -ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(-ap.width, 0, ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(-ap.width, totalH, ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-ap.width, totalH, -ap.depth);


        // ---------- RIGHT ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(ap.width, 0, -ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(ap.width, totalH, -ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(ap.width, totalH, ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, 0, ap.depth);


        // ---------- ROOF ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, totalH, -ap.depth);

        glTexCoord2f(0.0f, 4.0f);
        glVertex3f(-ap.width, totalH, ap.depth);

        glTexCoord2f(4.0f, 4.0f);
        glVertex3f(ap.width, totalH, ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, totalH, -ap.depth);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        // ================= WINDOWS =================

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, windowTexture);

        glColor3f(1.0f, 1.0f, 1.0f);

        for (int f = 0; f < ap.floors; f++) {

            float y = f * ap.floorHeight + 0.15f;

            for (int i = -3; i <= 3; i += 2) {

                float x1 = i * 0.6f - 0.2f;
                float x2 = i * 0.6f + 0.2f;

                float y1 = y;
                float y2 = y + 0.25f;

                glBegin(GL_QUADS);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(x1, y1, ap.depth + 0.01f);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(x2, y1, ap.depth + 0.01f);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(x2, y2, ap.depth + 0.01f);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(x1, y2, ap.depth + 0.01f);

                glEnd();
            }
        }

        glDisable(GL_TEXTURE_2D);


        // ================= DOOR =================

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, doorTexture);

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-0.5f, 0, ap.depth + 0.02f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(0.5f, 0, ap.depth + 0.02f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.5f, 0.8f, ap.depth + 0.02f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-0.5f, 0.8f, ap.depth + 0.02f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
    if (ap.LOD == 2) {
        glPushMatrix();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float totalH = ap.floors * ap.floorHeight;

        // ================= BODY =================
        GLuint betonTexture = allTextures["beton"];
        GLuint windowTexture = allTextures["window"];
        GLuint doorTexture = allTextures["door"];

        // ================= BODY =================
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, betonTexture);

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);

        // ---------- FRONT ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, 0, ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, 0, ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(ap.width, totalH, ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-ap.width, totalH, ap.depth);


        // ---------- BACK ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, 0, -ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-ap.width, totalH, -ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(ap.width, totalH, -ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, 0, -ap.depth);


        // ---------- LEFT ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, 0, -ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(-ap.width, 0, ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(-ap.width, totalH, ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-ap.width, totalH, -ap.depth);


        // ---------- RIGHT ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(ap.width, 0, -ap.depth);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(ap.width, totalH, -ap.depth);

        glTexCoord2f(4.0f, 8.0f);
        glVertex3f(ap.width, totalH, ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, 0, ap.depth);


        // ---------- ROOF ----------
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-ap.width, totalH, -ap.depth);

        glTexCoord2f(0.0f, 4.0f);
        glVertex3f(-ap.width, totalH, ap.depth);

        glTexCoord2f(4.0f, 4.0f);
        glVertex3f(ap.width, totalH, ap.depth);

        glTexCoord2f(4.0f, 0.0f);
        glVertex3f(ap.width, totalH, -ap.depth);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        // ================= WINDOWS =================

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, windowTexture);

        glColor3f(1.0f, 1.0f, 1.0f);

        for (int f = 0; f < ap.floors; f++) {

            float y = f * ap.floorHeight + 0.15f;

            for (int i = -3; i <= 3; i += 2) {

                float x1 = i * 0.6f - 0.2f;
                float x2 = i * 0.6f + 0.2f;

                float y1 = y;
                float y2 = y + 0.25f;

                glBegin(GL_QUADS);

                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(x1, y1, ap.depth + 0.01f);

                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(x2, y1, ap.depth + 0.01f);

                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(x2, y2, ap.depth + 0.01f);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(x1, y2, ap.depth + 0.01f);

                glEnd();
            }
        }

        glDisable(GL_TEXTURE_2D);


        // ================= DOOR =================

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, doorTexture);

        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-0.5f, 0, ap.depth + 0.02f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(0.5f, 0, ap.depth + 0.02f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.5f, 0.8f, ap.depth + 0.02f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-0.5f, 0.8f, ap.depth + 0.02f);

        glEnd();

        glDisable(GL_TEXTURE_2D);


        // ================= FIRE MARKS =================

        glColor4f(0.05f, 0.05f, 0.05f, 0.6f);
        glBegin(GL_QUADS);
        glVertex3f(-0.8f, 0.8f, ap.depth + 0.03f);
        glVertex3f(0.8f, 0.8f, ap.depth + 0.03f);
        glVertex3f(0.6f, 1.8f, ap.depth + 0.03f);
        glVertex3f(-0.6f, 1.8f, ap.depth + 0.03f);
        glEnd();

        for (int i = 0; i < 6; i++) {

            float x = ((rand() % 100) / 100.0f - 0.5f) * ap.width * 1.5f;
            float y = ((rand() % 100) / 100.0f) * totalH;
            float size = 0.2f + (rand() % 100) / 400.0f;

            glColor4f(0.0f, 0.0f, 0.0f, 0.4f);

            glBegin(GL_QUADS);
            glVertex3f(x - size, y - size, ap.depth + 0.03f);
            glVertex3f(x + size, y - size, ap.depth + 0.03f);
            glVertex3f(x + size, y + size, ap.depth + 0.03f);
            glVertex3f(x - size, y + size, ap.depth + 0.03f);
            glEnd();
        }


        // ================= DARK OVERLAY =================
        glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
        glBegin(GL_QUADS);
        glVertex3f(-ap.width, 0, ap.depth + 0.02f);
        glVertex3f(ap.width, 0, ap.depth + 0.02f);
        glVertex3f(ap.width, totalH, ap.depth + 0.02f);
        glVertex3f(-ap.width, totalH, ap.depth + 0.02f);
        glEnd();

        glDisable(GL_BLEND);

        glPopMatrix();
    }
}
void RenderSystem(std::vector<SmokeEffect*>& smokes) {
    for (auto e : entities) {
        if (healths[e].destroyed && renders[e].type != RenderType::Apartment && 
            renders[e].type != RenderType::Tank) continue;

        auto& t = transforms[e];
        auto& r = renders[e];

        glPushMatrix();
        glTranslatef(t.pos.x, t.pos.y, t.pos.z);
        glRotatef(t.angle, 0, 1, 0);

        switch (r.type) {
        case RenderType::Tank: {
            auto& tank = tanks[e];
            float bodyH = 0.6f;

            if (!tank.destroyed) drawTank(tank, bodyH);
            else drawDestroyedTank(tank, bodyH);
        }
                             break;

        case RenderType::Radar:
            glColor3f(0.4f, 0.4f, 0.4f);
            DrawCube(0.8f, 0.3f, 0.8f);
            break;

        case RenderType::Apartment: {
            auto& ap = apartments[e];
            float totalH = ap.floors * ap.floorHeight;

            if (ap.destroyed) {
                drawDestroyedAppartament(ap, totalH);
                drawSootEffect(0.0f, 0.1f, 0.0f, 40.0f, 5.0f);
                if (!ap.smokeEnabled) { smokes.push_back(new SmokeEffect(transforms[e].pos, 2000, 3.0f)); ap.smokeEnabled = true; }
            }
            else drawAppartament(ap, totalH);

        }
                                  break;
        }

        glPopMatrix();
    }
}
std::string getRenderTypeString(RenderType& type) {
    return rendersMap[type];
}
void BoundsSystem() {
    for (auto e : entities) {
        if (!transforms.contains(e) || !renders.contains(e)) continue;

        auto& t = transforms[e];
        auto& r = renders[e];

        Bounds b{};

        switch (r.type) {
        case RenderType::Tank:
            b = { t.pos.x - 1, t.pos.y - 0.6f, t.pos.z - 1,
                  t.pos.x + 1, t.pos.y + 1.5f, t.pos.z + 2.8f };
            break;

        case RenderType::Radar:
            b = { t.pos.x - 1, t.pos.y, t.pos.z - 1,
                  t.pos.x + 1, t.pos.y + 2, t.pos.z + 1 };
            break;

        case RenderType::Apartment: {
            auto& ap = apartments[e];
            float h = ap.floors * ap.floorHeight;
            b = { t.pos.x - ap.width, t.pos.y, t.pos.z - ap.depth,
                  t.pos.x + ap.width, t.pos.y + h, t.pos.z + ap.depth };
        }
                                  break;
        }

        bounds[e] = b;
    }
}
void HealthBarSystem() {
    glEnable(GL_LIGHTING);

    if (showBars) {
        for (auto& [entity, hp] : healths) {
            if (hp.destroyed) continue;

            auto& t = transforms[entity];
            float step = (apartments.contains(entity)) ? apartments[entity].floorHeight *
                apartments[entity].floors + 2.0f : 2.0f;

            std::string text = std::format("{}/{}", hp.current, hp.max);

            float stepZ = 0.0f;

            RenderTextWorld(t.pos.x, t.pos.y + step * 1.3f, t.pos.z, 1, 1, 1, getRenderTypeString(renders[entity].type).c_str());
            RenderTextWorld(t.pos.x, t.pos.y + step, t.pos.z, 1, 0, 0, text.c_str());
        }
    }
}
void DeathSystem(Tank& tank) {
    std::vector<Entity> toDelete;

    for (auto& [entity, hp] : healths) {
        if (hp.current <= 0 && !hp.destroyed) {
            hp.destroyed = true;

            if (apartments.contains(entity)) apartments[entity].destroyed = true;
            else if (tanks.contains(entity)) tanks[entity].destroyed = true;
            else toDelete.push_back(entity);

            LOG_INFO("+1 kill");

            tank.kills++;
        }
    }

    for (auto& e : toDelete) {
        healths.erase(e);
        renders.erase(e);
        radars.erase(e);
        bounds.erase(e);
        transforms.erase(e);

        auto it = std::find(entities.begin(), entities.end(), e);
        if (it != entities.end()) entities.erase(it);
    }
}
void Update(float dt, Tank& tank, Sound& sound) {
    RadarSystem(dt);
    BoundsSystem();
    DeathSystem(tank);

    for (auto& [id, bot] : tanks) {
        svbmath::Vec3 enemyPos = transforms[id].pos;
        svbmath::Vec3 tankPos = tank.pos;

        if (bot.finishReload > 0.0f) {
            bot.finishReload -= dt;
            if (bot.finishReload < 0.0f) bot.finishReload = 0.0f;
        }
            
        if (playerInRadius(enemyPos,tankPos, bot.detectionRadius) && !bot.destroyed) {
            svbmath::Vec3 dir = svbmath::Normalize(tankPos - enemyPos);
            
            float targetYaw = atan2(dir.x, -dir.z);
            float delta = svbmath::NormalizeAngle(targetYaw - bot.turretAngle);
            float newTarget = bot.turretAngle + delta;

            float speedMultiplier = 1.0f + fabs(delta) * 2.0f;
            float speed = bot.turretSpeed * speedMultiplier;
            speed = std::min(speed, bot.turretSpeed * 4.0f);

            bot.turretAngle = svbmath::RotateTowards(bot.turretAngle,
                newTarget, speed, dt);

            if (bot.finishReload <= 0.0f) {
                spawnShell({ enemyPos.x, enemyPos.y + 1.0f, enemyPos.z }, bot.turretAngle * 180.0f / PI, bot.gunAngle,
                    shellType::APFSDS, 100.0f, true);

                sound.setSourcePosition(sound.sources["Shot"], enemyPos);
                alSourceStop(sound.sources["Shot"]);
                alSourcePlay(sound.sources["Shot"]);

                LOG_INFO("Bot shooted");

                bot.finishReload = bot.reloadTime;
            }
        }
    }
}
void Render(std::vector<SmokeEffect*>& smokes, bool healthBar) {
    RenderSystem(smokes);
    if (healthBar) HealthBarSystem();
}
void generateEnemyes(std::unordered_map<int, Entity>& enemyes, int count) {
    entities.reserve(50);
    
    LOG_INFO("Genarate enemies");

    for (int i = 0; i < count; i++) {
        Entity e = CreateEntity();

        enemyes[i] = e;

        float x = (float)(rand() % 200 - 100);
        float z = -(float)(rand() % 200);

        transforms[e] = { x, 0.0f, z, (float)(rand() % 360) };

        if (i < 25) {
            renders[e] = { RenderType::Tank };
            healths[e] = { 200,200,15,false };
            tanks[e] = { 6.5f, 0.0f, 0.0f,0.0f,35.0f,40.0f };
        }
        else if (i < 30) {
            renders[e] = { RenderType::Radar };
            healths[e] = { 120,120,30,false };
            radars[e] = { 90.0f };
        }
        else {
            renders[e] = { RenderType::Apartment };
            healths[e] = { 1000,1000,50,false };
            apartments[e] = { 5 + rand() % 10, 0.6f, 4.0f, 2.0f };
        }
    }
}
checkCol checkCollisionWithTank(svbmath::Vec3& pos) {
    for (const auto& b : bounds) {
        if (checkCollision(b.second, pos)) {
            return {true, b.first};
        }
    }
    return { false, 0 };
}
bool playerInRadius(const svbmath::Vec3& enemyPos, const svbmath::Vec3& playerPos, float radius){
    svbmath::Vec3 diff = playerPos - enemyPos;
    return LengthSq(diff) <= radius * radius;
}