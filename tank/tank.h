#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <map>
#include "projectile.h"
#include "bounds.h"
#include "svbmath.h"

constexpr int MAX_TRACK_POINTS = 128;

struct TrackPoint {
    svbmath::Vec3 pos;
    float life; // 1.0 -> 0.0
};

struct TrackBuffer {
    TrackPoint points[MAX_TRACK_POINTS];
    int head = 0;
    int count = 0;
    svbmath::Vec3 lastPos;  
    bool hasLast = false;    

    void push(const svbmath::Vec3& p);
    TrackPoint& get(int i);
};
struct TankParams {
    // Hull
    float hullW = 3.5f;
    float hullH = 0.8f;
    float hullD = 2.5f;
    float hullY = 0.4f;

    // Tracks
    float trackW = 3.5f;
    float trackH = 0.4f;
    float trackD = 0.3f;
    float trackOffsetZ = 1.3f;
    float trackY = 0.2f;

    // Turret
    float turretW = 1.8f;
    float turretH = 0.6f;
    float turretD = 1.8f;
    float turretY = 0.9f;

    // Gun
    float gunRadius = 0.1f;
    float gunLength = 2.2f;
    float gunOffsetY = 0.1f;
    float gunOffsetZ = 0.9f;
};
class Tank {
public:
    TankParams params;

    std::map<int, svbmath::Vec3> spawns = {
     {1, {0.0f, 0.0f, 0.0f}},
     {2, {-30.0f, 0.0f, 0.0f}},
     {3, {30.0f, 0.0f, 0.0f}}
    };

    const float SPEED_LIMIT_FORWARD = 15.0f;
    const float SPEED_LIMIT_BACK = -7.0f;
    const float REDUCTION_COEF = 0.995f;
    const float VELOCITY_COEF = 0.2f;

    int selectedSpawn = 0;
    int HP = 200;
    int currentHP = HP;

    float baseX = 0.0f, baseY = 0.0f, baseZ = 0.0f;
    float x = 0.0f, y = 0.0f, z = 0.0f;
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

    void UpdateTrack(TrackBuffer& track, const svbmath::Vec3& tankPos, float dt);
    void Draw();
    void updateDirrections(float bR, float bY);
    void updatePosition(float x, float z, float dt);
    float returnImpactImpulse();
    svbmath::Vec3 RotateY(const svbmath::Vec3& v, float angleDeg);
    svbmath::Vec3 LocalToWorldTurret(const svbmath::Vec3& local);
    Bounds GetHullMax() const;
    void DrawTrack(const TrackBuffer& track, float width);
    void UpdateTrack(float dt, svbmath::Vec3 tankPos, svbmath::Vec3 tankRight, TrackBuffer& leftTrack, TrackBuffer& rightTrack);
private:
    void DrawBox(float w, float h, float d);
    void DrawCylinder(float r, float h, int seg = 16);
    void DrawHull();
    void DrawTracks();
    void DrawTurret();
    void DrawGun();
};
