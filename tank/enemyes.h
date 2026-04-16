#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "bounds.h"
#include "svbmath.h"
#include <map>

class SmokeEffect;
class Tank;
class Sound;

using Entity = uint32_t;

struct Transform {
    svbmath::Vec3 pos;
    float angle;
};
struct Health {
    int current;
    int max;
    float armor;
    bool destroyed = false;
};

enum class RenderType {
    Soldat,
    Vehicle,
    Tank,
    Radar,
    Apartment
};

extern std::map<RenderType, std::string> rendersMap;
extern bool showBars;

struct RenderComponent {
    RenderType type;
};
struct TankComponent {
    float reloadTime = 6.5f, finishReload = 0.0f;

    float turretAngle = 0.0f;
    float gunAngle = 0.0f;
    float detectionRadius = 35.0f;
    float turretSpeed = 70.0f;

    bool destroyed = false;
};
struct RadarComponent {
    float rotationSpeed;
};
struct ApartmentComponent {
    int floors;
    float floorHeight;
    float width;
    float depth;
    bool destroyed = false;
    bool smokeEnabled = false;
    int LOD = 1;
};


extern std::vector<Entity> entities;

extern std::unordered_map<Entity, Transform> transforms;
extern std::unordered_map<Entity, Health> healths;
extern std::unordered_map<Entity, RenderComponent> renders;
extern std::unordered_map<Entity, TankComponent> tanks;
extern std::unordered_map<Entity, RadarComponent> radars;
extern std::unordered_map<Entity, ApartmentComponent> apartments;
extern std::unordered_map<Entity, Bounds> bounds;

Entity CreateEntity();
void drawTank(TankComponent& tank, float bodyH);
void DrawCube(float w, float h, float d);
void RadarSystem(float dt);
void drawDestroyedTank(TankComponent& tank, float bodyH);
void drawDestroyedAppartament(ApartmentComponent& ap, float totalH);
void drawAppartament(ApartmentComponent& ap, float totalH);
void RenderSystem(std::vector<SmokeEffect*>& smokes);
void BoundsSystem();
void HealthBarSystem();
void DeathSystem(Tank& tank);
void Update(float dt, Tank& tank,Sound& sound);
void Render(std::vector<SmokeEffect*>& smokes,bool healthBar = true);
void generateEnemyes(std::unordered_map<int, Entity>& enemyes, int count);
std::string getRenderTypeString(RenderType& type);
bool playerInRadius(const svbmath::Vec3& enemyPos, const svbmath::Vec3& playerPos, float radius);
checkCol checkCollisionWithTank(svbmath::Vec3& pos);