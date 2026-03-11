#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "Info.h"
#include <unordered_map>
#include "effects.h"
#include "text.h"
#include "tank.h"
#include "enemyes.h"

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

void drawTank(TankComponent& tank, float bodyH) {
    glPushMatrix();
    glRotatef(1, 0, 1, 0);
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
    glRotatef(tank.turretAngle, 0, 1, 0);

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
void RadarSystem(float dt) {
    for (auto& [entity, radar] : radars) {
        transforms[entity].angle += radar.rotationSpeed * dt;
        if (transforms[entity].angle >= 360.0f) transforms[entity].angle -= 360.0f;
    }
}
void drawDestroyedAppartament(ApartmentComponent& ap, float totalH) {
    float halfW = ap.width;
    float halfD = ap.depth;

    float leftTop = totalH;
    float rightTop = totalH * 0.5f;

    glPushMatrix();

    // ================= FRONT =================
    glColor3f(0.55f, 0.55f, 0.55f);

    // Левая часть
    glBegin(GL_QUADS);
    glVertex3f(-halfW, 0, halfD);
    glVertex3f(0.0f, 0, halfD);
    glVertex3f(0.0f, leftTop, halfD);
    glVertex3f(-halfW, leftTop, halfD);
    glEnd();

    // Правая разрушенная часть
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0, halfD);
    glVertex3f(halfW, 0, halfD);
    glVertex3f(halfW, rightTop, halfD);
    glVertex3f(0.0f, rightTop, halfD);
    glEnd();


    // ================= BACK =================
    glBegin(GL_QUADS);
    glVertex3f(-halfW, 0, -halfD);
    glVertex3f(halfW, 0, -halfD);
    glVertex3f(halfW, rightTop, -halfD);
    glVertex3f(-halfW, leftTop, -halfD);
    glEnd();


    // ================= LEFT WALL =================
    glBegin(GL_QUADS);
    glVertex3f(-halfW, 0, -halfD);
    glVertex3f(-halfW, 0, halfD);
    glVertex3f(-halfW, leftTop, halfD);
    glVertex3f(-halfW, leftTop, -halfD);
    glEnd();


    // ================= RIGHT WALL =================
    glBegin(GL_QUADS);
    glVertex3f(halfW, 0, -halfD);
    glVertex3f(halfW, 0, halfD);
    glVertex3f(halfW, rightTop, halfD);
    glVertex3f(halfW, rightTop, -halfD);
    glEnd();


    // ================= BROKEN TOP =================
    glColor3f(0.35f, 0.35f, 0.35f);

    glBegin(GL_TRIANGLES);

    // Верхняя наклонная поверхность
    glVertex3f(-halfW, leftTop, halfD);
    glVertex3f(-halfW, leftTop, -halfD);
    glVertex3f(halfW, rightTop, -halfD);

    glVertex3f(-halfW, leftTop, halfD);
    glVertex3f(halfW, rightTop, -halfD);
    glVertex3f(halfW, rightTop, halfD);

    glEnd();


    // ================= WINDOWS (стабильные) =================
    glColor3f(0.1f, 0.1f, 0.1f);

    for (int f = 0; f < ap.floors; f++)
    {
        float y = f * ap.floorHeight + 0.2f;
        if (y > rightTop) break; // окна только на целой части

        for (int i = -2; i <= 0; i++)
        {
            float x = i * 0.7f;

            glBegin(GL_QUADS);
            glVertex3f(x - 0.2f, y, halfD + 0.01f);
            glVertex3f(x + 0.2f, y, halfD + 0.01f);
            glVertex3f(x + 0.2f, y + 0.3f, halfD + 0.01f);
            glVertex3f(x - 0.2f, y + 0.3f, halfD + 0.01f);
            glEnd();
        }
    }


    // ================= CROOKED DOOR =================
    glPushMatrix();
    glTranslatef(-0.5f, 0, halfD + 0.02f);
    glRotatef(-10, 0, 0, 1);

    glColor3f(0.25f, 0.15f, 0.08f);
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(1.0f, 0, 0);
    glVertex3f(1.0f, 1.0f, 0);
    glVertex3f(0, 1.0f, 0);
    glEnd();

    glPopMatrix();


    // ================= RUBBLE (стабильная позиция) =================
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
        glRotatef(1, 0, 1, 0);

        // ================= BODY =================
        glColor3f(0.75f, 0.75f, 0.7f);
        glBegin(GL_QUADS);
        // Front
        glVertex3f(-ap.width, 0, ap.depth);
        glVertex3f(ap.width, 0, ap.depth);
        glVertex3f(ap.width, totalH, ap.depth);
        glVertex3f(-ap.width, totalH, ap.depth);

        // Back
        glVertex3f(-ap.width, 0, -ap.depth);
        glVertex3f(-ap.width, totalH, -ap.depth);
        glVertex3f(ap.width, totalH, -ap.depth);
        glVertex3f(ap.width, 0, -ap.depth);

        // Left
        glVertex3f(-ap.width, 0, -ap.depth);
        glVertex3f(-ap.width, 0, ap.depth);
        glVertex3f(-ap.width, totalH, ap.depth);
        glVertex3f(-ap.width, totalH, -ap.depth);

        // Right
        glVertex3f(ap.width, 0, -ap.depth);
        glVertex3f(ap.width, totalH, -ap.depth);
        glVertex3f(ap.width, totalH, ap.depth);
        glVertex3f(ap.width, 0, ap.depth);

        // Roof
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-ap.width, totalH, -ap.depth);
        glVertex3f(-ap.width, totalH, ap.depth);
        glVertex3f(ap.width, totalH, ap.depth);
        glVertex3f(ap.width, totalH, -ap.depth);

        glEnd();

        // ================= WINDOWS =================
        glColor3f(0.2f, 0.4f, 0.8f);
        for (int f = 0; f < ap.floors; f++) {
            float y = f * ap.floorHeight + 0.15f;
            for (int i = -3; i <= 3; i += 2) {
                glBegin(GL_QUADS);
                glVertex3f(i * 0.6f - 0.2f, y, ap.depth + 0.01f);
                glVertex3f(i * 0.6f + 0.2f, y, ap.depth + 0.01f);
                glVertex3f(i * 0.6f + 0.2f, y + 0.25f, ap.depth + 0.01f);
                glVertex3f(i * 0.6f - 0.2f, y + 0.25f, ap.depth + 0.01f);
                glEnd();
            }
        }

        // ================= DOORS =================
        glColor3f(0.3f, 0.2f, 0.1f);
        glBegin(GL_QUADS);
        glVertex3f(-0.5f, 0, ap.depth + 0.02f);
        glVertex3f(0.5f, 0, ap.depth + 0.02f);
        glVertex3f(0.5f, 0.8f, ap.depth + 0.02f);
        glVertex3f(-0.5f, 0.8f, ap.depth + 0.02f);
        glEnd();

        glPopMatrix();
    }
    if (ap.LOD == 2) {
        glPushMatrix();
        glRotatef(1, 0, 1, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float totalH = ap.floors * ap.floorHeight;

        // ================= BODY =================
        glColor3f(0.75f, 0.7f, 0.65f);
        glBegin(GL_QUADS);

        // Front
        glVertex3f(-ap.width, 0, ap.depth);
        glVertex3f(ap.width, 0, ap.depth);
        glVertex3f(ap.width, totalH, ap.depth);
        glVertex3f(-ap.width, totalH, ap.depth);

        // Back
        glVertex3f(-ap.width, 0, -ap.depth);
        glVertex3f(-ap.width, totalH, -ap.depth);
        glVertex3f(ap.width, totalH, -ap.depth);
        glVertex3f(ap.width, 0, -ap.depth);

        // Left
        glVertex3f(-ap.width, 0, -ap.depth);
        glVertex3f(-ap.width, 0, ap.depth);
        glVertex3f(-ap.width, totalH, ap.depth);
        glVertex3f(-ap.width, totalH, -ap.depth);

        // Right
        glVertex3f(ap.width, 0, -ap.depth);
        glVertex3f(ap.width, totalH, -ap.depth);
        glVertex3f(ap.width, totalH, ap.depth);
        glVertex3f(ap.width, 0, ap.depth);

        // Roof
        glColor3f(0.5f, 0.45f, 0.45f);
        glVertex3f(-ap.width, totalH, -ap.depth);
        glVertex3f(-ap.width, totalH, ap.depth);
        glVertex3f(ap.width, totalH, ap.depth);
        glVertex3f(ap.width, totalH, -ap.depth);

        glEnd();


        // ================= WINDOWS =================
        for (int f = 0; f < ap.floors; f++) {
            float y = f * ap.floorHeight + 0.15f;

            for (int i = -3; i <= 3; i += 2) {

                float x1 = i * 0.6f - 0.2f;
                float x2 = i * 0.6f + 0.2f;
                float y2 = y + 0.25f;

                int state = rand() % 3;

                if (state == 0) {
                    // нормальное окно
                    glColor3f(0.2f, 0.4f, 0.8f);
                    glBegin(GL_QUADS);
                    glVertex3f(x1, y, ap.depth + 0.01f);
                    glVertex3f(x2, y, ap.depth + 0.01f);
                    glVertex3f(x2, y2, ap.depth + 0.01f);
                    glVertex3f(x1, y2, ap.depth + 0.01f);
                    glEnd();
                }
                else if (state == 1) {
                    // выбитое окно (черная дыра)
                    glColor3f(0.05f, 0.05f, 0.05f);
                    glBegin(GL_QUADS);
                    glVertex3f(x1, y, ap.depth + 0.01f);
                    glVertex3f(x2, y, ap.depth + 0.01f);
                    glVertex3f(x2, y2, ap.depth + 0.01f);
                    glVertex3f(x1, y2, ap.depth + 0.01f);
                    glEnd();
                }
                else {
                    // разбитое стекло (осколок)
                    glColor3f(0.2f, 0.4f, 0.8f);
                    glBegin(GL_TRIANGLES);
                    glVertex3f(x1, y, ap.depth + 0.01f);
                    glVertex3f(x2, y, ap.depth + 0.01f);
                    glVertex3f(x1, y2, ap.depth + 0.01f);
                    glEnd();

                    glColor3f(0.05f, 0.05f, 0.05f);
                    glBegin(GL_TRIANGLES);
                    glVertex3f(x2, y, ap.depth + 0.01f);
                    glVertex3f(x2, y2, ap.depth + 0.01f);
                    glVertex3f(x1, y2, ap.depth + 0.01f);
                    glEnd();
                }
            }
        }


        // ================= DOOR =================
        glColor3f(0.3f, 0.2f, 0.1f);
        glBegin(GL_QUADS);
        glVertex3f(-0.5f, 0, ap.depth + 0.02f);
        glVertex3f(0.5f, 0, ap.depth + 0.02f);
        glVertex3f(0.5f, 0.8f, ap.depth + 0.02f);
        glVertex3f(-0.5f, 0.8f, ap.depth + 0.02f);
        glEnd();


        // ================= FIRE MARKS =================

        // копоть над дверью
        glColor4f(0.05f, 0.05f, 0.05f, 0.6f);
        glBegin(GL_QUADS);
        glVertex3f(-0.8f, 0.8f, ap.depth + 0.03f);
        glVertex3f(0.8f, 0.8f, ap.depth + 0.03f);
        glVertex3f(0.6f, 1.8f, ap.depth + 0.03f);
        glVertex3f(-0.6f, 1.8f, ap.depth + 0.03f);
        glEnd();

        // случайные пятна копоти
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
        if (healths[e].destroyed && renders[e].type != RenderType::Apartment) continue;

        auto& t = transforms[e];
        auto& r = renders[e];

        glPushMatrix();
        glTranslatef(t.x, t.y, t.z);
        glRotatef(t.angle, 0, 1, 0);

        switch (r.type) {
        case RenderType::Soldat:
            glColor3f(0, 1, 0);
            DrawCube(0.3f, 0.7f, 0.3f);
            break;

        case RenderType::Vehicle:
            glColor3f(0, 0.8f, 0);
            DrawCube(1.0f, 0.6f, 1.0f);
            break;

        case RenderType::Tank: {
            auto& tank = tanks[e];
            float bodyH = 0.6f;

            drawTank(tank, bodyH);
        }
                             break;

        case RenderType::Radar:
            glColor3f(0.4f, 0.4f, 0.4f);
            DrawCube(0.8f, 0.3f, 0.8f);
            break;

        case RenderType::Apartment: {
            auto& ap = apartments[e];
            float totalH = ap.floors * ap.floorHeight;
            float x = transforms[e].x;
            float y = transforms[e].y;
            float z = transforms[e].z;

            if (ap.destroyed) {
                drawDestroyedAppartament(ap, totalH);
                if (!ap.smokeEnabled) { smokes.push_back(new SmokeEffect(x, y, z, 2000, 3.0f)); ap.smokeEnabled = true; }
            }
            else drawAppartament(ap, totalH);

        }
                                  break;
        }

        glPopMatrix();
    }
}
void BoundsSystem() {
    for (auto e : entities) {
        if (!transforms.contains(e) || !renders.contains(e)) continue;

        auto& t = transforms[e];
        auto& r = renders[e];

        Bounds b{};

        switch (r.type) {
        case RenderType::Soldat:
            b = { t.x - 0.5f, t.y, t.z - 0.5f,
                  t.x + 0.5f, t.y + 1.5f, t.z + 0.5f };
            break;

        case RenderType::Vehicle:
            b = { t.x - 1, t.y - 0.6f, t.z - 1,
                  t.x + 1, t.y + 0.6f, t.z + 1 };
            break;

        case RenderType::Tank:
            b = { t.x - 1, t.y - 0.6f, t.z - 1,
                  t.x + 1, t.y + 1.5f, t.z + 2.8f };
            break;

        case RenderType::Radar:
            b = { t.x - 1, t.y, t.z - 1,
                  t.x + 1, t.y + 2, t.z + 1 };
            break;

        case RenderType::Apartment: {
            auto& ap = apartments[e];
            float h = ap.floors * ap.floorHeight;
            b = { t.x - ap.width, t.y, t.z - ap.depth,
                  t.x + ap.width, t.y + h, t.z + ap.depth };
        }
                                  break;
        }

        bounds[e] = b;
    }
}
void HealthBarSystem() {
    for (auto& [entity, hp] : healths) {
        if (hp.destroyed) continue;

        auto& t = transforms[entity];
        float step = (apartments.contains(entity)) ? apartments[entity].floorHeight *
            apartments[entity].floors + 2.0f : 2.0f;

        std::string text = std::format("{}/{}", hp.current, hp.max);

        RenderTextWorld(t.x, t.y + step, t.z, 1, 0, 0, text.c_str());
    }
}
void DeathSystem(Tank& tank) {
    for (auto& [entity, hp] : healths) if (hp.current <= 0) {
        if (hp.current <= 0 && !hp.destroyed) {
            hp.destroyed = true;

            if (apartments.contains(entity)) apartments[entity].destroyed = true;

            tank.kills++;
        }
    }
}
void Update(float dt, Tank& tank) {
    RadarSystem(dt);
    BoundsSystem();
    DeathSystem(tank);
}
void Render(std::vector<SmokeEffect*>& smokes, bool healthBar) {
    RenderSystem(smokes);
    if (healthBar) HealthBarSystem();
}

void generateEnemyes(std::unordered_map<int, Entity>& enemyes, int count) {
    for (int i = 0; i < count; i++) {
        Entity e = CreateEntity();

        enemyes[i] = e;

        float x = (float)(rand() % 200 - 100);
        float z = -(float)(rand() % 200);

        transforms[e] = { x, 0.0f, z, (float)(rand() % 360) };

        if (i < 10) { renders[e] = { RenderType::Vehicle }; healths[e] = { 100,100,false }; }
        else if (i < 20) { renders[e] = { RenderType::Soldat }; healths[e] = { 50,50,false }; }
        else if (i < 25) {
            renders[e] = { RenderType::Tank };
            healths[e] = { 200,200,false };
            tanks[e] = { 0.0f, 0.0f };
        }
        else if (i < 30) {
            renders[e] = { RenderType::Radar };
            healths[e] = { 120,120,false };
            radars[e] = { 90.0f };
        }
        else {
            renders[e] = { RenderType::Apartment };
            healths[e] = { 1000,1000,false };
            apartments[e] = { 5 + rand() % 10, 0.6f, 4.0f, 2.0f };
        }
    }
}