#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "svbmath.h" 

void DrawSpawnMarker2D(const svbmath::Vec3& pos, float size = 1.0f, float r = 1.0f, float g = 0.0f, float b = 0.0f) {
    glPushMatrix();

    glTranslatef(pos.x, pos.y + 0.05f, pos.z);

    glColor3f(r, g, b);

    glBegin(GL_LINES);
    glVertex3f(-size * 0.5f, 0.0f, -size * 0.5f);
    glVertex3f(size * 0.5f, 0.0f, size * 0.5f);

    glVertex3f(-size * 0.5f, 0.0f, size * 0.5f);
    glVertex3f(size * 0.5f, 0.0f, -size * 0.5f);
    glEnd();

    glPopMatrix();
}