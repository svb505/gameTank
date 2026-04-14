#pragma once
#include <vector>
#include "Projectile.h"
#include "enemyes.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <map>
#include <algorithm>
#include "effects.h"
#include "Info.h"
#include "sounds.h"
#include "shells.h"
#include "artillery.h"
#include "svbmath.h"


struct Info;
struct EffectsContext;

extern std::map<shellType,std::string> shellTypes;

extern std::vector<Projectile> projectiles;

void spawnShell(svbmath::Vec3 pos, float yawDeg, float pitchDeg, shellType _shellType, int shellSpeed,
    bool isEnemy = false);
void spawnBullet(svbmath::Vec3 pos, float yawDeg);
void update(float dt, Sound& sound, std::unordered_map<int, Entity>& enemies, std::unordered_map<Entity, Health>& healths,
    std::unordered_map<Entity, Bounds>& bounds, EffectsContext& context, Tank& player);
void updateProjectiles();
void updateArtillery(std::vector<Projectile>& artilleryProjectiles, Sound& sound,
    std::unordered_map<int, Entity>& enemies, EffectsContext& context);
std::string getShellType(shellType& shellType);
void onHit(Projectile& p, int id, Health* health, EffectsContext& context, Sound& sound, Tank& player, bool hitGround);
float calculatePenetration(float vel);
