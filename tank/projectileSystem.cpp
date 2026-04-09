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
#include "projectileSystem.h"
#include "tank.h"
#include "Logger.h"
#include "text.h"

float ProjectileSystem::calculatePenetration(float vel) {
    const float k = 0.0005f;
    return k * vel * vel;
}
void ProjectileSystem::onHit(Projectile& p, Entity& en, Health& health, EffectsContext& context, Sound& sound,
    bool hitGround, bool smokeShell) {

    sound.setSourcePosition(sound.sources["Explosion"], p.x, p.y, p.z);
    alSourceStop(sound.sources["Explosion"]);
    alSourcePlay(sound.sources["Explosion"]);

    float x = p.x;
    float y = p.y;
    float z = p.z;

    if (!hitGround) {
        health.current -= p.damage;
        if (health.current <= 0) health.destroyed = true;
    }

    if (p.type == ProjectileType::Shell) {
        int count, radius = 0;
        float height = 0.0f;

        if (!smokeShell) {
            if (p.selectedShellType == shellType::APFSDS) { count = 300; radius = 4; height = 1.5f; }
            else { count = 500; radius = 6; height = 2.0f; }

            context.explosions.push_back(new ExplosionEffect(x, y, z, count, radius, height, 1.8f));
            context.smokes.push_back(new SmokeEffect(x, y, z, 300, 3));
        }
        else context.smokes.push_back(new SmokeEffect(x, y, z, 5500, 6, { 1.0f,1.0f,1.0f,0.3f }, 3.0f, 0.01f, 6.0f));
    }
}
void ProjectileSystem::spawnShell(float x, float y, float z, float yawDeg, float pitchDeg, shellType _shellType, 
    int shellSpeed,bool isEnemy) {
    Projectile p(_shellType);
    p.type = ProjectileType::Shell;

    p.selectedShellType = _shellType;

    p.x = x; p.y = y; p.z = z;
    p.speed = shellSpeed;

    float yaw = yawDeg * 3.1415926f / 180.0f;
    float pitch = pitchDeg * 3.1415926f / 180.0f;
    if (_shellType == shellType::ATGM) {
        svbmath::Vec3 dir{ sin(yaw) * cos(pitch),sin(pitch), cos(yaw) * cos(pitch)};
        float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        dir.x /= len; dir.y /= len; dir.z /= len;

        p.dir = dir;
        p.turnSpeed = 5.0f;
    }
    else {
        p.vx = sin(yaw) * cos(pitch) * p.speed;
        p.vy = -sin(pitch) * p.speed;
        p.vz = cos(yaw) * cos(pitch) * p.speed;
    }
    
    p.isEnemy = isEnemy;

    LOG_INFO("Player shooted");

    projectiles.push_back(p);
}
void ProjectileSystem::spawnBullet(float x, float y, float z, float yawDeg) {
    Projectile p(shellType::BULLET);
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
void ProjectileSystem::update(float dt, Sound& sound, std::unordered_map<int, Entity>& enemies, std::unordered_map<Entity, Health>& healths,
    std::unordered_map<Entity, Bounds>& bounds, EffectsContext& context,Tank& player) {

    for (auto& p : projectiles) {
        if (!p.alive) continue;

        p.update(dt, player);

        if (checkCollision(player.GetHullMax(), p.x, p.y, p.z) && p.isEnemy) {
            player.currentHP -= p.damage;

            p.alive = false;

            sound.setSourcePosition(sound.sources["Explosion"], p.x, p.y, p.z);
            alSourceStop(sound.sources["Explosion"]);
            alSourcePlay(sound.sources["Explosion"]);

            if (p.type != ProjectileType::Bullet) context.explosions.push_back(new ExplosionEffect(p.x, p.y, p.z));
            if (p.selectedShellType == shellType::SMOKE) context.smokes.push_back(new SmokeEffect(p.x, p.y, p.z));

            if (player.currentHP <= 0) {
                player.currentHP = player.HP;

                player.x = player.spawns[player.selectedSpawn].x; 
                player.y = player.spawns[player.selectedSpawn].y;
                player.z = player.spawns[player.selectedSpawn].z;
            }

            break;
        }

        for (auto& [id, en] : enemies) {
            if (!healths.contains(id)) continue;
            if (healths[id].destroyed && renders[id].type != RenderType::Apartment) continue;
            if (!bounds.contains(id)) continue;

            if (checkCollision(bounds[id], p.x, p.y, p.z) && !p.isEnemy && calculatePenetration(p.speed)) {
                healths[id].current -= p.damage;

                if (healths[id].current > 0.0f) g_destroyText = "Target hit";
                else { 
                    g_destroyText = "Target Destoyed";
                    sound.setSourcePosition(sound.sources["Kill"],player.x,player.y,player.z);
                    alSourcePlay(sound.sources["Kill"]);
                }
                if (healths[id].current <= healths[id].max / 2) {
                    if (apartments.contains(id)) apartments[id].LOD = 2;
                }

                if (apartments.contains(id) && p.type != ProjectileType::Bullet) {
                    context.smokes.push_back(new SmokeEffect(p.x, p.y, p.z, 300, 1.5f, { 0.5f,0.5f,0.5f }, 2.0f));
                }

                sound.setSourcePosition(sound.sources["Explosion"], p.x, p.y, p.z);
                alSourceStop(sound.sources["Explosion"]);
                alSourcePlay(sound.sources["Explosion"]);

                if (p.type != ProjectileType::Bullet) context.explosions.push_back(new ExplosionEffect(p.x, p.y, p.z));
                if (p.selectedShellType == shellType::SMOKE) context.smokes.push_back(new SmokeEffect(p.x, p.y, p.z));

                p.alive = false;
                break;
            }
            if (p.alive && p.y <= 0.0f && p.type != ProjectileType::Bullet && !p.isEnemy) {
                onHit(p, en, healths[id], context, sound, p.y <= 0.0f, p.selectedShellType == shellType::SMOKE);

                if (p.selectedShellType == shellType::SMOKE) context.smokes.push_back(new SmokeEffect(p.x, p.y, p.z));

                context.explosions.push_back(new ExplosionEffect(p.x, p.y, p.z));

                p.alive = false;
            }
        }

    }
    std::erase_if(projectiles, [](const Projectile& p) { return !p.alive; });
}
void ProjectileSystem::updateProjectiles(ProjectileSystem& projectileSystem) {
    for (auto& p : projectileSystem.projectiles) {
        if (!p.alive) continue;

        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);

        if (p.type == ProjectileType::Shell) drawShell();
        else drawBullet();

        glPopMatrix();
    }
}
void ProjectileSystem::updateArtillery(std::vector<Projectile>& artilleryProjectiles, Sound& sound,
    std::unordered_map<int, Entity>& enemies, EffectsContext& context) {
    for (auto& p : artilleryProjectiles) {
        if (!p.alive) continue;

        bool exploded = false;

        for (auto& [id, en] : enemies) {
            if (!healths.contains(id) || healths[id].destroyed) continue;
            if (!bounds.contains(id)) continue;

            if (checkCollision(bounds[id], p.x, p.y, p.z) && calculatePenetration(p.speed)) {
                healths[id].current -= p.damage;

                context.explosions.push_back(new ExplosionEffect(p.x, p.y, p.z, 200));

                p.alive = false;
                exploded = true;
                sound.setSourcePosition(sound.sources["ArtExplosion"], p.x, p.y, p.z);
                alSourcePlay(sound.sources["ArtExplosion"]);
                break;
            }
        }
        if (!exploded && p.y <= 0.0f) {
            context.explosions.push_back(new ExplosionEffect(p.x, p.y, p.z, 200));
            sound.setSourcePosition(sound.sources["ArtExplosion"], p.x, p.y, p.z);
            alSourcePlay(sound.sources["ArtExplosion"]);
            p.alive = false;
        }
    }
}