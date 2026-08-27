#pragma once
#include "svbmath.h"

class Camera;
class Tank;

void DrawSpawnMarker2D(const svbmath::Vec3& pos, bool selected, float size = 1.0f, float r = 1.0f, float g = 0.0f, float b = 0.0f);
void drawRadar(const svbmath::Vec2& pos, float size, float angle);
void drawTank(const svbmath::Vec2& pos, float size, float angle);
void drawHouse(const svbmath::Vec2& pos, float size);
void drawPlayerIcon(const svbmath::Vec2& pos, float size, float angle);
void Begin2D(int width, int height);
void End2D();

void WorldToScreen(float worldX, float worldZ, float centerX, float centerZ,float yaw, const Camera& cam, float scale, float screenW, float screenH, float& outX,
    float& outY);

void RenderBadges(int ww, int wh, float scale,Tank& tank,Camera& cam);