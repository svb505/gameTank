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
#include "projectileSystem.h"
#include "tank.h"
#include "Logger.h"
#include "text.h"
#include "craters.h"
#include "killchat.h"

std::vector<Projectile> projectiles = {};
std::map<shellType, std::string> shellTypes = { {shellType::APFSDS,"APFSDS"},{shellType::HE,"HE"},
        {shellType::SMOKE,"SMOKE"} ,{shellType::ATGM,"ATGM"} ,{shellType::BULLET,"BULLET"} };

std::string getShellType(shellType& shellType) {
    return shellTypes[shellType];
}

float calculatePenetration(float vel) {
    const float k = 0.0005f;
    return k * vel * vel;
}
void onHit(Projectile& p, int id, Health* health,EffectsContext& context,Sound& sound, Tank& player,bool hitGround) {

    const char* snd = (p.type == ProjectileType::Shell) ? "Explosion" : "Explosion";
    sound.setSourcePosition(sound.sources[snd], p.pos);
    alSourceStop(sound.sources[snd]);
    alSourcePlay(sound.sources[snd]);

    if (!hitGround && health) {
        bool wasAlive = health->current > 0.0f;

        if (calculatePenetration(p.speed) >= health->armor) {
            health->current -= p.damage;

            if (wasAlive && health->current <= 0.0f) {
                player.kills++;

                health->destroyed = true;
                g_destroyText = "Target Destroyed";

                if (!p.isEnemy && p.type != ProjectileType::Bullet)
                    player.score += player.scoreToCount;

                addToKillChat("Player",getRenderTypeString(renders[id].type),getShellType(p.selectedShellType),0,id);

                sound.setSourcePosition(sound.sources["Kill"], player.pos);
                alSourcePlay(sound.sources["Kill"]);
            }
            else if (health->current > 0.0f) {
                g_destroyText = "Target hit";

                if (!p.isEnemy && p.type != ProjectileType::Bullet) player.score += player.scoreToCount / 2;
            }
        }

        if (health->current <= health->max / 2) {
            if (apartments.contains(id)) apartments[id].LOD = 2;      
        }
        if (health->current <= 0) {
            if (apartments.contains(id)) {
                apartments[id].destroyed = true;
            }
            if (tanks.contains(id)) tanks[id].destroyed = true;
        }
    }

    if (hitGround) addCrater(p.pos.x, p.pos.z);

    if (p.type != ProjectileType::Bullet) {

        if (p.selectedShellType == shellType::SMOKE) {
            context.smokes.push_back(
                new SmokeEffect(p.pos , 5500, 6, { 1.0f,1.0f,1.0f,0.3f }, 3.0f, 0.01f, 6.0f)
            );
        }
        else {
            int count = (p.selectedShellType == shellType::APFSDS) ? 300 : 500;
            int radius = (p.selectedShellType == shellType::APFSDS) ? 4 : 6;
            float height = (p.selectedShellType == shellType::APFSDS) ? 1.5f : 2.0f;

            context.explosions.push_back(
                new ExplosionEffect(p.pos, count, radius, height, 1.8f)
            );

            context.smokes.push_back(
                new SmokeEffect(p.pos, 300, 3)
            );
        }
    }

    p.alive = false;
}
void spawnShell(svbmath::Vec3 pos, float yawDeg, float pitchDeg, shellType _shellType,
    int shellSpeed,bool isEnemy) {
    Projectile p(_shellType);
    p.type = ProjectileType::Shell;

    p.selectedShellType = _shellType;

    p.pos.x = pos.x; p.pos.y = pos.y; p.pos.z = pos.z;
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
    else p.velocity = { sin(yaw) * cos(pitch) * p.speed, -sin(pitch) * p.speed,cos(yaw) * cos(pitch) * p.speed };
    
    p.isEnemy = isEnemy;

    LOG_INFO("Player shooted");

    projectiles.push_back(p);
}
void spawnBullet(svbmath::Vec3 pos, float yawDeg) {
    Projectile p(shellType::BULLET);
    p.type = ProjectileType::Bullet;
    p.pos.x = pos.x; p.pos.y = pos.y; p.pos.z = pos.z;
    p.speed = 120.0f;
    p.damage = 1;
    p.lifeTime = 4.0f;

    float yaw = yawDeg * 3.1415926f / 180.0f;

    p.velocity = { sin(yaw) * p.speed, 0.0f, cos(yaw) * p.speed };

    projectiles.push_back(p);
}
void update(float dt,Sound& sound,std::unordered_map<int, Entity>& enemies,std::unordered_map<Entity, Health>& healths,
    std::unordered_map<Entity, Bounds>& bounds,EffectsContext& context,Tank& player) {
    for (auto& p : projectiles) {
        if (!p.alive) continue;

        p.update(dt, player);

        if (checkCollision(player.GetHullMax(), p.pos) && p.isEnemy) {

            player.currentHP -= p.damage;

            onHit(p, 0, nullptr, context, sound, player, false);

            if (player.currentHP <= 0) {
                addToKillChat("Tank", "Player", getShellType(p.selectedShellType), 0, 0);

                player.death++;
                player.currentHP = player.HP;
                player.pos.x = player.spawns[player.selectedSpawn].x;
                player.pos.y = player.spawns[player.selectedSpawn].y;
                player.pos.z = player.spawns[player.selectedSpawn].z;
            }

            continue;
        }

        for (auto& [id, en] : enemies) {

            if (!healths.contains(id)) continue;
            if (!bounds.contains(id)) continue;
            if (healths[id].destroyed && renders[id].type != RenderType::Apartment) continue;

            if (checkCollision(bounds[id], p.pos) && !p.isEnemy) {

                onHit(p,id, &healths[id], context, sound, player, false);
                break;
            }
        }
        if (p.alive && p.pos.y <= 0.0f && p.type != ProjectileType::Bullet) {
            onHit(p, 0, nullptr, context, sound, player, true);
        }
    }

    std::erase_if(projectiles, [](const Projectile& p) {
        return !p.alive;
        });
}
void updateProjectiles() {
    for (auto& p : projectiles) {
        if (!p.alive) continue;

        glPushMatrix();
        glTranslatef(p.pos.x, p.pos.y, p.pos.z);

        if (p.type == ProjectileType::Shell) drawShell();
        else drawBullet();

        glPopMatrix();
    }
}
void updateArtillery(std::vector<Projectile>& artilleryProjectiles, Sound& sound,
    std::unordered_map<int, Entity>& enemies, EffectsContext& context) {
    for (auto& p : artilleryProjectiles) {
        if (!p.alive) continue;

        bool exploded = false;

        for (auto& [id, en] : enemies) {
            if (!healths.contains(id) || healths[id].destroyed) continue;
            if (!bounds.contains(id)) continue;

            if (checkCollision(bounds[id], p.pos) && calculatePenetration(p.speed)) {
                healths[id].current -= p.damage;

                context.explosions.push_back(new ExplosionEffect(p.pos, 200));

                p.alive = false;
                exploded = true;
                sound.setSourcePosition(sound.sources["ArtExplosion"], p.pos);
                alSourcePlay(sound.sources["ArtExplosion"]);
                break;
            }
        }
        if (!exploded && p.pos.y <= 0.0f) {
            addCrater(p.pos.x, p.pos.z);

            context.explosions.push_back(new ExplosionEffect(p.pos, 200));
            sound.setSourcePosition(sound.sources["ArtExplosion"], p.pos);
            alSourcePlay(sound.sources["ArtExplosion"]);
            p.alive = false;
        }
    }
}