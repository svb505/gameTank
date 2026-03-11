#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

class SmokeEffect;
class Tank;

using Entity = uint32_t;

struct Transform {
    float x, y, z;
    float angle;
};
struct Health {
    int current;
    int max;
    bool destroyed = false;
};
enum class RenderType {
    Soldat,
    Vehicle,
    Tank,
    Radar,
    Apartment
};
struct RenderComponent {
    RenderType type;
};
struct TankComponent {
    float turretAngle;
    float gunAngle;
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
struct Bounds {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
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
void drawDestroyedAppartament(ApartmentComponent& ap, float totalH);
void drawAppartament(ApartmentComponent& ap, float totalH);
void RenderSystem(std::vector<SmokeEffect*>& smokes);
void BoundsSystem();
void HealthBarSystem();
void DeathSystem(Tank& tank);
void Update(float dt, Tank& tank);
void Render(std::vector<SmokeEffect*>& smokes,bool healthBar = true);
void generateEnemyes(std::unordered_map<int, Entity>& enemyes, int count);