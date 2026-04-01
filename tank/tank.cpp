#pragma once
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cfloat>   
#include <algorithm> 
#include "projectile.h"
#include "bounds.h"
#include "svbmath.h"
#include "tank.h"

void Tank::Draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(bodyYaw, 0, 1, 0);

    DrawHull();
    DrawTracks();
    DrawTurret();

    glPopMatrix();
}
void Tank::updateDirrections(float bR, float bY) {
    bodyRad = (bY + 90.0f) * 3.1415926f / 180.0f;
    dirX = -sin(bR);
    dirZ = -cos(bR);
}
void Tank::updatePosition(float x, float z, float dt) {
    this->x += dirX * moveSpeed * dt;
    this->z += dirZ * moveSpeed * dt;
}
float Tank::returnImpactImpulse() {
    float baseDmg = 100;
    float speedFactor = 1.0f + moveSpeed / SPEED_LIMIT_FORWARD;

    return baseDmg * speedFactor;
}
void Tank::DrawBox(float w, float h, float d) {
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
void Tank::DrawCylinder(float r, float h, int seg) {
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
void Tank::DrawHull() {
    glColor3f(0.2f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0, 0.4f, 0);
    DrawBox(3.5f, 0.8f, 2.5f);
    glPopMatrix();
}
void Tank::DrawTracks() {
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(0, 0.2f, side * 1.3f);

        glColor3f(0.1f, 0.1f, 0.1f);
        DrawBox(3.5f, 0.4f, 0.3f);
        glPopMatrix();
    }
}
void Tank::DrawTurret() {
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
void Tank::DrawGun() {
    glPushMatrix();
    glTranslatef(0, 0.1f, 0.9f);
    glRotatef(gunPitch, 1, 0, 0);

    glColor3f(0.1f, 0.1f, 0.1f);
    glRotatef(90, 1, 0, 0);
    DrawCylinder(0.1f, 2.2f);

    glPopMatrix();
}
Bounds Tank::GetHullMax() const {
    Bounds result;

    float hx = 3.5f / 2.0f;
    float hy = 0.8f / 2.0f;
    float hz = 2.5f / 2.0f;
    float offsetY = 0.4f;

    float cosYaw = cos(bodyYaw * 3.1415926f / 180.0f);
    float sinYaw = sin(bodyYaw * 3.1415926f / 180.0f);

    svbmath::Vec3 corners[8] = {
        {-hx, -hy + offsetY, -hz},
        {-hx, -hy + offsetY, hz},
        {-hx, hy + offsetY, -hz},
        {-hx, hy + offsetY, hz},
        {hx, -hy + offsetY, -hz},
        {hx, -hy + offsetY, hz},
        {hx, hy + offsetY, -hz},
        {hx, hy + offsetY, hz},
    };

    svbmath::Vec3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
    svbmath::Vec3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int i = 0; i < 8; ++i) {
        float x = corners[i].x * cosYaw - corners[i].z * sinYaw;
        float z = corners[i].x * sinYaw + corners[i].z * cosYaw;
        float y = corners[i].y;

        min.x = std::min(min.x, x + this->x);
        min.y = std::min(min.y, y + this->y);
        min.z = std::min(min.z, z + this->z);

        max.x = std::max(max.x, x + this->x);
        max.y = std::max(max.y, y + this->y);
        max.z = std::max(max.z, z + this->z);
    }

    result.minX = min.x;
    result.minY = min.y;
    result.minZ = min.z;
    result.maxX = max.x;
    result.maxY = max.y;
    result.maxZ = max.z;


    return result;
}