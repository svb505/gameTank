#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "environnement.h"
#include <cmath>
#include <string>
#include <format>
#include "text.h"
#include "getnormal.h"
#include "getheight.h"
#include "craters.h"

const float size = 100.0f;

void drawGridText(float camX, float camZ) {
    float baseX = camX;
    float baseZ = camZ;
    float step = 50.0f;

    for (float i = -size / 100; i <= size / 100; i += step) {
        for (float j = -size / 100; j <= size / 100; j += step) {
            float x = baseX + i;
            float z = baseZ + j;

            std::string text = std::format("(X:{:.0f}; Z:{:.0f})", x, z);
            float y = 1.5f;
            RenderTextWorld(x, y, z, 0.8f, 0.8f, 0.8f, text.c_str());
        }
    }
}
void drawGrid(float camX, float camZ) {
    float baseX = camX;
    float baseZ = camZ;
    float step = 50.0f;

    glColor4f(0.8f, 0.8f, 0.8f, 0.3f);
    glBegin(GL_LINES);

    for (float i = -size; i <= size; i += step) {

        for (float j = -size; j <= size; j += step) {
            float x = baseX + i;
            float z = baseZ + j;

            float y = 0.2f;

            // линии по X
            if (j < size) {
                float z2 = baseZ + j + step;
                float y2 = 0.2f;

                glVertex3f(x, y, z);
                glVertex3f(x, y2, z2);
            }

            // линии по Z
            if (i < size) {
                float x2 = baseX + i + step;
                float y2 = 0.2f;

                glVertex3f(x, y, z);
                glVertex3f(x2, y2, z);
            }
        }
    }

    glEnd();
}
void drawSky() {
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(128.0f / 255, 128.0f / 255, 128.0f / 255);
    glVertex2f(-1, 1);
    glVertex2f(1, 1);

    glColor3f(160.0f / 255, 160.0f / 255, 160.0f / 255);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}
void drawGround(float camX, float camZ, std::string weather) {
    float step = 2.0f;
    float x0 = camX;
    float z0 = camZ;

    for (float x = -size; x < size; x += step) {
        for (float z = -size; z < size; z += step) {
            float x1 = x0 + x;
            float z1 = z0 + z;

            float x2 = x1 + step;
            float z2 = z1 + step;

            float y1 = getHeight(x1, z1);
            float y2 = getHeight(x2, z1);
            float y3 = getHeight(x2, z2);
            float y4 = getHeight(x1, z2);

            glBegin(GL_QUADS);

            auto calcColor = [&](float vx, float vz) {
                float r = 0.0f, g = 102.0f / 255, b = 0.0f;

                bool inCrater = false;
                for (auto& c : craters) {
                    float dx = vx - c.x;
                    float dz = vz - c.z;
                    float dist = sqrt(dx * dx + dz * dz);

                    if (dist < c.radius) {
                        inCrater = true;
                        break;
                    }
                }

                if (weather == "Snowly") {
                    if (inCrater) {
                        r = 200.0f / 255;
                        g = 180.0f / 255;
                        b = 150.0f / 255;
                    }
                    else {
                        r = 1.0f; g = 1.0f; b = 1.0f;
                    }
                }
                else {
                    if (inCrater) {
                        r = 153.0f / 255;
                        g = 76.0f / 255;
                        b = 0.0f;
                    }
                    else {
                        r = 0.0f; g = 102.0f / 255; b = 0.0f;
                    }
                }

                glColor3f(r, g, b);
                };


            auto n1 = getNormal(x1, z1); glNormal3f(n1.x, n1.y, n1.z); calcColor(x1, z1); glVertex3f(x1, y1, z1);
            auto n2 = getNormal(x2, z1); glNormal3f(n2.x, n2.y, n2.z); calcColor(x2, z1); glVertex3f(x2, y2, z1);
            auto n3 = getNormal(x2, z2); glNormal3f(n3.x, n3.y, n3.z); calcColor(x2, z2); glVertex3f(x2, y3, z2);
            auto n4 = getNormal(x1, z2); glNormal3f(n4.x, n4.y, n4.z); calcColor(x1, z2); glVertex3f(x1, y4, z2);

            glEnd();
        }
    }
}