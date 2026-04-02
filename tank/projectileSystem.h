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

struct Info;

class ProjectileSystem {
public:
    std::vector<Projectile> projectiles;

    void spawnShell(float x, float y, float z, float yawDeg, float pitchDeg, shellType _shellType, int shellSpeed,
        bool isEnemy = false);
    void spawnBullet(float x, float y, float z, float yawDeg);
    void update(float dt, Sound& sound, std::unordered_map<int, Entity>& enemies, std::unordered_map<Entity, Health>& healths,
        std::unordered_map<Entity, Bounds>& bounds, std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes,Tank& player);
    void updateProjectiles(ProjectileSystem& projectileSystem);
    void updateArtillery(std::vector<Projectile>& artilleryProjectiles, Sound& sound,
        std::unordered_map<int, Entity>& enemies, std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes);
private:
    void onHit(Projectile& p, Entity& en, Health& health, std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes, Sound& sound, bool hitGround = false, bool smokeShell = false);
    float calculatePenetration(float vel);
};
