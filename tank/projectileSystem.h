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

struct Info;
class ProjectileSystem {
public:
    std::vector<Projectile> projectiles;

    void spawnShell(float x, float y, float z,float yawDeg, float pitchDeg,shellType _shellType,int shellSpeed)
    {
        Projectile p;
        p.type = ProjectileType::Shell;

        p.selectedShellType = _shellType;
        
        p.x = x; p.y = y; p.z = z;
        p.speed = shellSpeed;
        p.damage = (p.selectedShellType == shellType::APFSDS) ? 100 : 40;

        float yaw = yawDeg * 3.1415926f / 180.0f;
        float pitch = pitchDeg * 3.1415926f / 180.0f;

        p.vx = sin(yaw) * cos(pitch) * p.speed;
        p.vy = -sin(pitch) * p.speed;
        p.vz = cos(yaw) * cos(pitch) * p.speed;

        projectiles.push_back(p);
    }
    void spawnBullet(float x, float y, float z,float yawDeg)
    {
        Projectile p;
        p.type = ProjectileType::Bullet;
        p.x = x; p.y = y; p.z = z;
        p.speed = 120.0f;
        p.damage = 1;
        p.lifeTime = 4.0f;

        float yaw = yawDeg * 3.1415926f / 180.0f;
        p.vx = sin(yaw) * p.speed;
        p.vy = 0.0f;
        p.vz = cos(yaw) * p.speed;

        projectiles.push_back(p);
    }
    void update(float dt,std::map<int, Info>& enemyes,std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes,ALuint explosionSource)
    {
        for (auto& p : projectiles) {
            if (!p.alive) continue;
            p.update(dt);

            for (auto& en : enemyes) {
                Enemy* enemy = en.second.enemy;
                if (!enemy || enemy->IsDestroyed()) continue;

                if (checkCollision(enemy, p)) {
                    onHit(p, en, explosions, smokes, explosionSource);
                    p.alive = false;
                    break;
                }
                if (p.y <= 0) {
                    onHit(p, en, explosions, smokes, explosionSource,true,
                        (p.selectedShellType == shellType::SMOKE));
                    p.alive = false;
                    break;
                }
            }
        }

        std::erase_if(projectiles,
            [](const Projectile& p) { return !p.alive; });
    }

private:
    bool checkCollision(Enemy* enemy, const Projectile& p)
    {
        float minX = enemy->vertices[0][0], maxX = minX;
        float minY = enemy->vertices[0][1], maxY = minY;
        float minZ = enemy->vertices[0][2], maxZ = minZ;

        for (auto& v : enemy->vertices) {
            minX = std::min(minX, v[0]);
            maxX = std::max(maxX, v[0]);
            minY = std::min(minY, v[1]);
            maxY = std::max(maxY, v[1]);
            minZ = std::min(minZ, v[2]);
            maxZ = std::max(maxZ, v[2]);
        }

        return p.x >= minX && p.x <= maxX &&
            p.y >= minY && p.y <= maxY &&
            p.z >= minZ && p.z <= maxZ;
    }
    void onHit(Projectile& p, std::pair<const int, Info>& en, std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes, ALuint explosionSource, bool hitGround = false,bool smokeShell = false)
    {
        alSourceStop(explosionSource);
        alSourcePlay(explosionSource);

        float x = p.x;
        float y = p.y;
        float z = p.z;

        if (!hitGround) {
            Enemy* enemy = en.second.enemy;
            enemy->health -= p.damage;
            if (enemy->health <= 0) {
                enemy->SetDestroyed(true);
            }
        }

        if (p.type == ProjectileType::Shell) {
            int count,radius = 0;
            float height = 0.0f;

            if (!smokeShell) {
                if (p.selectedShellType == shellType::APFSDS) { count = 300; radius = 4; height = 1.5f; }
                else { count = 500; radius = 6; height = 2.0f; }

                explosions.push_back(
                    new ExplosionEffect(x, y, z, count, radius, height, 1.8f));
                smokes.push_back(
                    new SmokeEffect(x, y, z, 300, 3));
            }
            else {
                smokes.push_back(
                    new SmokeEffect(x, y, z, 5500, 6,{1.0f,1.0f,1.0f,1.0f},3.0f,0.01f,6.0f));
            }
            
        }

        
    }
};
