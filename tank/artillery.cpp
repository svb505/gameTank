#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <random>
#include <unordered_map>
#include "artillery.h"
#include "enemyes.h"
#include "projectileSystem.h"
#include "projectile.h"
#include "Logger.h"

std::vector<float> Artillery::returnRandomError() {
    std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> errX(0.0f,15.0f);
    std::uniform_real_distribution<float> errZ(0.0f,15.0f);

    return {errX(gen),errZ(gen)};
}
float Artillery::returnSpeedError() {
    std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> errSpeed(0.5f,1.0f);

    return errSpeed(gen);
}
void Artillery::drawShell() {
    float s = 0.1f;
    glBegin(GL_QUADS);
    // front
    glVertex3f(-s, -s, s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(-s, s, s);
    // back
    glVertex3f(-s, -s, -s); glVertex3f(s, -s, -s); glVertex3f(s, s, -s); glVertex3f(-s, s, -s);
    // left
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s); glVertex3f(-s, s, s); glVertex3f(-s, s, -s);
    // right
    glVertex3f(s, -s, -s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(s, s, -s);
    // top
    glVertex3f(-s, s, -s); glVertex3f(-s, s, s); glVertex3f(s, s, s); glVertex3f(s, s, -s);
    // bottom
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s); glVertex3f(s, -s, s); glVertex3f(s, -s, -s);
    glEnd();
}
void Artillery::spawnShells(float x,float z) {
    LOG_INFO("Spawned artillery shell");

    for (int i = 0; i < volley; i++) {
        auto error = returnRandomError();
        Projectile s(shellType::HE);
        s.x = x + error[0];
        s.y = shellHeight;
        s.z = z + error[1];
        s.delay = 5.0f;
        s.active = false;
        s.speed = shellSpeed * returnSpeedError();
        shells.push_back(s);
    }
}
void Artillery::updateShells(float dt) {
    for (auto& s : shells) {
        if (!s.alive) continue;

        if (!s.active) {
            s.delay -= dt;
            if (s.delay <= 0.0f) s.active = true;
            continue;
        }

        s.y -= s.speed * dt;
    }
}
void Artillery::drawAllShells() {
    for (auto& s : shells) {
        if (!s.alive) continue;
        glPushMatrix();
        glTranslatef(s.x, s.y, s.z);
        drawShell();
        glPopMatrix();
    }
}
void Artillery::deleteIfAlived() {
    for (auto it = shells.begin();it != shells.end();) {
        if (!(*it).alive) it = shells.erase(it);
        else ++it;
    }
}