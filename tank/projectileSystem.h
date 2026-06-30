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
#include "sounds.h"
#include "shells.h"
#include "artillery.h"
#include "svbmath.h"
#include "cameraShake.h"

struct EffectsContext;

extern std::map<shellType,std::string> shellTypes;

extern std::vector<Projectile> projectiles;

void reserveProjectiles();
void spawnShell(svbmath::Vec3 pos, float yawDeg, float pitchDeg, shellType _shellType, int shellSpeed,
    bool isEnemy = false);

void spawnBullet(svbmath::Vec3 pos, float yawDeg);

void update(float dt, Sound& sound, std::vector<Projectile>& artilleryProjectiles, 
    ECSCompenents& components, EffectsContext& context, Tank& player,CameraShake& shake);

void drawProjectiles();

std::string getShellType(shellType& shellType);

void onHit(Projectile& p, int id, Health* health, EffectsContext& context, Sound& sound, Tank& player, bool hitGround);
float calculatePenetration(float vel);
