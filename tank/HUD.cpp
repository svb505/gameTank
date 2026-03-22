#include "HUD.h"
#include "projectile.h"
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "text.h"
#include <format>
#include "tank.h"
#include <vector>


void HUD::Draw3DAim(Tank& tank) {
    float turretRad = (tank.bodyYaw + tank.turretYaw) * 3.1415926f / 180.0f;
    float pitchRad = tank.gunPitch * 3.1415926f / 180.0f;
    float barrelLength = 4.9f;
    float barrelHeight = 0.9f;

    float barrelX = tank.x + sin(turretRad) * cos(pitchRad) * barrelLength;
    float barrelY = tank.y + barrelHeight - sin(pitchRad) * barrelLength;
    float barrelZ = tank.z + cos(turretRad) * cos(pitchRad) * barrelLength;

    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    float size = 1.0f;

    glBegin(GL_LINES);
    glVertex3f(barrelX - size, barrelY, barrelZ);
    glVertex3f(barrelX + size, barrelY, barrelZ);
    glVertex3f(barrelX, barrelY - size, barrelZ);
    glVertex3f(barrelX, barrelY + size, barrelZ);
    glEnd();

    glEnable(GL_LIGHTING);
    glPopAttrib();
}