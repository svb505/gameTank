#pragma once
#include "svbmath.h"

class Camera;
class Tank;

void DrawSpawnMarker2D(const svbmath::Vec3& pos, float size = 1.0f, float r = 1.0f, float g = 0.0f, float b = 0.0f);
void drawSoldier(float x, float y, float size);
void drawCar(float x, float y, float size);
void drawRadar(float x, float y, float size, float angle);
void drawTank(float x, float y, float size, float angle);
void drawHouse(float x, float y, float size);
void drawPlayerIcon(float x, float y, float size, float angle);
void Begin2D(int width, int height);
void End2D();
void WorldToScreen(float worldX, float worldZ, float centerX, float centerZ,float yaw, const Camera& cam, float scale, float screenW, float screenH, float& outX,
    float& outY);
void RenderBadges(int ww, int wh, float scale,Tank& tank,Camera cam);