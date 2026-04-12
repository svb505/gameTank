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

void Tank::UpdateTrack(TrackBuffer& track, const svbmath::Vec3& pos, float dt) {
    float minDist = 0.3f;

    if (!track.hasLast) {
        track.lastPos = pos;
        track.hasLast = true;
    }

    if (svbmath::Length(pos - track.lastPos) > minDist) {
        track.push(pos);
        track.lastPos = pos;
    }

    for (int i = 0; i < track.count; i++) track.get(i).life -= dt * 0.2f;
}
TrackPoint& TrackBuffer::get(int i) {
    int idx = (head - count + i + MAX_TRACK_POINTS) % MAX_TRACK_POINTS;
    return points[idx];
}
void TrackBuffer::push(const svbmath::Vec3& p) {
    points[head] = { p, 1.0f };
    head = (head + 1) % MAX_TRACK_POINTS;
    if (count < MAX_TRACK_POINTS) count++;
}
svbmath::Vec3 Tank::RotateY(const svbmath::Vec3& v, float angleDeg) {
    float a = angleDeg * PI / 180.0f;
    float c = cos(a);
    float s = sin(a);

    return {v.x * c - v.z * s,v.y,v.x * s + v.z * c};
}
svbmath::Vec3 Tank::LocalToWorldTurret(const svbmath::Vec3& local) {
    svbmath::Vec3 p = local;

    p = RotateY(p, turretYaw);
    p.y += params.turretY;
    p = RotateY(p, bodyYaw);

    p.x += x;
    p.y += y;
    p.z += z;

    return p;
}
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
    glTranslatef(0, params.hullY, 0);
    DrawBox(params.hullW, params.hullH, params.hullD);
    glPopMatrix();
}
void Tank::DrawTracks() {
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(0, params.trackY, side * params.trackOffsetZ);

        glColor3f(0.1f, 0.1f, 0.1f);
        DrawBox(params.trackW, params.trackH, params.trackD);

        glPopMatrix();
    }
}
void Tank::DrawTurret() {
    glPushMatrix();
    glTranslatef(0, params.turretY, 0);
    if (turretLocked) glRotatef(turretYaw, 0, 1, 0);
    else glRotatef(turretYaw - bodyYaw - 90.0f, 0, 1, 0);

    glColor3f(0.15f, 0.4f, 0.15f);

    glPushMatrix();
    DrawBox(params.turretW, params.turretH, params.turretD);
    glPopMatrix();

    DrawGun();

    glPopMatrix();
}
void Tank::DrawGun() {
    glPushMatrix();

    glTranslatef(0, params.gunOffsetY, params.gunOffsetZ);
    glRotatef(gunPitch, 1, 0, 0);

    glColor3f(0.1f, 0.1f, 0.1f);

    glRotatef(90, 1, 0, 0);
    DrawCylinder(params.gunRadius, params.gunLength, 16);

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
void Tank::UpdateTrack(float dt, svbmath::Vec3 tankPos, svbmath::Vec3 tankRight, TrackBuffer& leftTrack, TrackBuffer& rightTrack) {
    float offset = 1.5f;
    float TRACK_Y_OFFSET = 0.02f;

    svbmath::Vec3 leftPos = tankPos - tankRight * offset;
    svbmath::Vec3 rightPos = tankPos + tankRight * offset;

    leftPos.y += TRACK_Y_OFFSET;
    rightPos.y += TRACK_Y_OFFSET;

    UpdateTrack(leftTrack, leftPos, dt);
    UpdateTrack(rightTrack, rightPos, dt);
}
void Tank::DrawTrack(const TrackBuffer& track, float width) {
    if (track.count < 2) return;

    svbmath::Vec3 up = { 0, 1, 0 };

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);

    for (int i = 0; i < track.count - 1; i++) {
        const TrackPoint& p1 = track.points[
            (track.head - track.count + i + MAX_TRACK_POINTS) % MAX_TRACK_POINTS
        ];
        const TrackPoint& p2 = track.points[
            (track.head - track.count + i + 1 + MAX_TRACK_POINTS) % MAX_TRACK_POINTS
        ];

        if (p1.life <= 0.0f || p2.life <= 0.0f) continue;

        svbmath::Vec3 dir = svbmath::Normalize(p2.pos - p1.pos);
        svbmath::Vec3 right = svbmath::Cross(dir, up) * width;

        float alpha1 = p1.life;
        float alpha2 = p2.life;

        glColor4f(0.2f, 0.2f, 0.2f, alpha1);
        glVertex3f(p1.pos.x - right.x, p1.pos.y, p1.pos.z - right.z);
        glVertex3f(p1.pos.x + right.x, p1.pos.y, p1.pos.z + right.z);

        glColor4f(0.2f, 0.2f, 0.2f, alpha2);
        glVertex3f(p2.pos.x + right.x, p2.pos.y, p2.pos.z + right.z);
        glVertex3f(p2.pos.x - right.x, p2.pos.y, p2.pos.z - right.z);
    }

    glEnd();
}