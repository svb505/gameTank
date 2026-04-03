#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "environnement.h"
#include <cmath>
#include <string>
#include <format>
#include "text.h"

const float size = 10000.0f;

void drawGridText(float camX, float camZ) {
    float baseX = camX;
    float baseZ = camZ;
    float step = 50.0f;

    for (float i = -size / 100; i <= size / 100; i += step) {
        for (float j = -size / 100; j <= size / 100; j += step) {
            float x = baseX + i;
            float z = baseZ + j;

            std::string text = std::format("(X:{:.0f}; Z:{:.0f})", x, -z);
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
void drawGround(float camX, float camZ,std::string weather) {
    float x = camX;
    float z = camZ;

    if (weather == "Snowly") glColor3f(1.0f, 1.0f, 1.0f);
    else glColor3f(0.0f, 102.0f / 255, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(x - size, 0.0f, z - size);
    glVertex3f(x + size, 0.0f, z - size);
    glVertex3f(x + size, 0.0f, z + size);
    glVertex3f(x - size, 0.0f, z + size);
    glEnd();
}