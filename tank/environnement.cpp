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
#include "texture.h"
#include "weather.h"

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
void drawSky(){
    GLuint skyTex = allTextures["cloudy"];

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyTex);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);

    glColor3f(1, 1, 1);

    glTexCoord2f(0, 1); glVertex2f(-1, 1);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glTexCoord2f(1, 0); glVertex2f(1, -1);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);

    glEnd();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}
void drawGround(float camX, float camZ, Type& weather) {
    float step = 4.0f;
    float x0 = camX;
    float z0 = camZ;

    bool snowly = (weather == Type::snowy);

    glBegin(GL_QUADS);

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

            float centerX = (x1 + x2) * 0.5f;
            float centerZ = (z1 + z2) * 0.5f;

            bool inCrater = false;

            for (const auto& c : craters) {
                float dx = centerX - c.x;
                float dz = centerZ - c.z;

                if (dx * dx + dz * dz < c.radius * c.radius) {
                    inCrater = true;
                    break;
                }
            }

            if (snowly) {
                if (inCrater) glColor3f(200.0f / 255.0f, 180.0f / 255.0f, 150.0f / 255.0f);
                else glColor3f(1.0f, 1.0f, 1.0f);
            }
            else {
                if (inCrater) glColor3f(153.0f / 255.0f, 76.0f / 255.0f, 0.0f);
                else glColor3f(0.0f, 102.0f / 255.0f, 0.0f);
            }

            svbmath::Vec3 n1 = svbmath::Normalize(svbmath::Vec3(y1 - y2, 2.0f, y1 - y4));
            svbmath::Vec3 n2 = svbmath::Normalize(svbmath::Vec3(y1 - y2, 2.0f, y2 - y3));
            svbmath::Vec3 n3 = svbmath::Normalize(svbmath::Vec3(y4 - y3, 2.0f, y2 - y3));
            svbmath::Vec3 n4 = svbmath::Normalize(svbmath::Vec3(y4 - y3, 2.0f, y1 - y4));

            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(x1, y1, z1);

            glNormal3f(n2.x, n2.y, n2.z);
            glVertex3f(x2, y2, z1);

            glNormal3f(n3.x, n3.y, n3.z);
            glVertex3f(x2, y3, z2);

            glNormal3f(n4.x, n4.y, n4.z);
            glVertex3f(x1, y4, z2);
        }
    }

    glEnd();
}
