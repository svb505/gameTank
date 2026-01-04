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
void HUD::drawHud(int screenWidth, int screenHeight, Tank& tank,const int count) {
    std::string selectedShell = "";

    if (tank.selectedShell == shellType::APFSDS) selectedShell = "APFSDS";
    else if (tank.selectedShell == shellType::SMOKE) selectedShell = "SMOKE";
    else selectedShell = "HE";
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);


    RenderTextHUD(10, 15, 1, 1, 1, std::format("Total Shells: {}", tank.totalShells).c_str(), screenWidth, screenHeight);
    RenderTextHUD(10, 30, 1, 1, 1, std::format("Reload Time: {:.1f}", tank.finishReload).c_str(), screenWidth, screenHeight);
    RenderTextHUD(10, 80, 1, 1, 1, std::format("Selected shell: {}", selectedShell).c_str(), screenWidth, screenHeight);
    RenderTextHUD(10, 100, 1, 1, 1, "1 - Select APFSDS | 2 - Select HE | 3 - Select SMOKE", screenWidth, screenHeight);
    
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
