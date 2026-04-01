#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "projectile.h"
#include "bounds.h"
#include "svbmath.h"

class Tank {
public:
    const float SPEED_LIMIT_FORWARD = 15.0f;
    const float SPEED_LIMIT_BACK = -7.0f;
    const float REDUCTION_COEF = 0.995f;
    const float VELOCITY_COEF = 0.2f;

    int HP = 200;
    int currentHP = HP;

    float baseX = 0, baseY = 0, baseZ = 0;
    float x = 0, y = 0, z = 0;
    float oldX = x, oldY = y, oldZ = z;

    float bodyYaw = -90.0f, turretYaw = -90.0f, gunPitch = 0.0f;  

    int maxShells = 26, totalShells = 26;
    float reloadTime = 6.5f, finishReload = 0.0f;

    bool aimMode = false;
    int kills = 0;

    shellType selectedShell;
    int shellSpeed = 400;

    float moveSpeed = 0.0f, rotateSpeed = 60.0f, turretSpeed = 60.0f, gunSpeed = 30.0f;

    float bodyRad = (bodyYaw + 90.0f) * 3.1415926f / 180.0f, dirX = -sin(bodyRad), dirZ = -cos(bodyRad);

    void Draw();
    void updateDirrections(float bR, float bY);
    void updatePosition(float x, float z, float dt);
    float returnImpactImpulse();
    Bounds GetHullMax() const;
private:
    void DrawBox(float w, float h, float d);
    void DrawCylinder(float r, float h, int seg = 16);
    void DrawHull();
    void DrawTracks();
    void DrawTurret();
    void DrawGun();
};
