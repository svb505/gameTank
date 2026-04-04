#include "effects.h"
#include "smokeGranade.h"
#include <cmath>

void SmokeGranade::draw(float x, float y, float z) {
    float s = 0.1f;

    glPushMatrix();
    glTranslatef(x,y,z);

    glColor3f(0.3f, 0.3f, 0.3f);

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
    glPopMatrix();
}
void SmokeGranade::drawAll() {
    for (auto& g : granades) draw(g.pos.x, g.pos.y, g.pos.z);
}
void SmokeGranade::spawn(float x, float y, float z) {
    Position p;
}