#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <random>
#include "artillery.h"

std::vector<float> Artillery::returnRandomError() {
    std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> errX(0.0f,6.0f);
    std::uniform_real_distribution<float> errZ(0.0f, 6.0f);

    return {errX(gen),errZ(gen)};
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
    for (int i = 0; i < volley; i++) {
        auto error = returnRandomError();
        artShell s;
        s.x = x + error[0];
        s.y = shellHeight;
        s.z = z + error[1];

        shells.push_back(s);
    }
}
void Artillery::updateShells(float dt) {
    for (auto s : shells) {
        s.y -= shellSpeed * dt;
    }
}