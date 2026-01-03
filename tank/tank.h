#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "projectile.h"

constexpr float PI = 3.1415926f;

class Tank {
public:
    float x = 0, y = 0, z = 0;

    float bodyYaw = 0.0f;
    float turretYaw = 0.0f; 
    float gunPitch = 0.0f;  

    int totalShells = 26;
    float reloadTime = 6.5f;
    float finishReload = 0.0f;

    bool aimMode = false;

    shellType selectedShell;
    int shellSpeed = 400;

    void Draw() {
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(bodyYaw, 0, 1, 0);

        DrawHull();
        DrawTracks();
        DrawTurret();

        glPopMatrix();
    }

private:

    void DrawBox(float w, float h, float d) {
        glScalef(w, h, d);
        glBegin(GL_QUADS);
        // Front
        glVertex3f(-0.5, -0.5, 0.5);
        glVertex3f(0.5, -0.5, 0.5);
        glVertex3f(0.5, 0.5, 0.5);
        glVertex3f(-0.5, 0.5, 0.5);
        // Back
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f(-0.5, 0.5, -0.5);
        glVertex3f(0.5, 0.5, -0.5);
        glVertex3f(0.5, -0.5, -0.5);
        // Left
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f(-0.5, -0.5, 0.5);
        glVertex3f(-0.5, 0.5, 0.5);
        glVertex3f(-0.5, 0.5, -0.5);
        // Right
        glVertex3f(0.5, -0.5, -0.5);
        glVertex3f(0.5, 0.5, -0.5);
        glVertex3f(0.5, 0.5, 0.5);
        glVertex3f(0.5, -0.5, 0.5);
        // Top
        glVertex3f(-0.5, 0.5, -0.5);
        glVertex3f(-0.5, 0.5, 0.5);
        glVertex3f(0.5, 0.5, 0.5);
        glVertex3f(0.5, 0.5, -0.5);
        // Bottom
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f(0.5, -0.5, -0.5);
        glVertex3f(0.5, -0.5, 0.5);
        glVertex3f(-0.5, -0.5, 0.5);
        glEnd();
    }

    void DrawCylinder(float r, float h, int seg = 16) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= seg; i++) {
            float a = 2.0f * PI * i / seg;
            float x = cos(a) * r;
            float z = sin(a) * r;
            glVertex3f(x, -h / 2, z);
            glVertex3f(x, h / 2, z);
        }
        glEnd();
    }

    //body

    void DrawHull() {
        glColor3f(0.2f, 0.5f, 0.2f);
        glPushMatrix();
        glTranslatef(0, 0.4f, 0);
        DrawBox(3.5f, 0.8f, 2.5f);
        glPopMatrix();
    }

    void DrawTracks() {
        for (int side = -1; side <= 1; side += 2) {
            glPushMatrix();
            glTranslatef(0, 0.2f, side * 1.3f);

            glColor3f(0.1f, 0.1f, 0.1f);
            DrawBox(3.5f, 0.4f, 0.3f);
            glPopMatrix();
        }
    }

    void DrawTurret() {
        glPushMatrix();
        glTranslatef(0, 0.9f, 0);
        glRotatef(turretYaw, 0, 1, 0);

        // turret
        glColor3f(0.15f, 0.4f, 0.15f);
        glPushMatrix();
        DrawBox(1.8f, 0.6f, 1.8f);
        glPopMatrix();

        DrawGun();

        glPopMatrix();
    }

    //gun

    void DrawGun() {
        glPushMatrix();
        glTranslatef(0, 0.1f, 0.9f);
        glRotatef(gunPitch, 1, 0, 0);

        glColor3f(0.1f, 0.1f, 0.1f);
        glRotatef(90, 1, 0, 0);
        DrawCylinder(0.1f, 2.2f);

        glPopMatrix();
    }
};
