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
#include "texture.h"
#include "CameraShake.h"
#include "APSSystem.h"

std::vector<Projectile> projectiles = {};
std::map<shellType, int> shellSpeeds = { {shellType::APFSDS, 400},
                                            {shellType::HE, 100 }, 
                                            { shellType::ATGM, 10 },
                                            { shellType::SMOKE, 200 } };

std::map<shellType, std::string> shellTypes = { {shellType::APFSDS,"APFSDS"},{shellType::HE,"HE"},
        {shellType::SMOKE,"SMOKE"} ,{shellType::ATGM,"ATGM"} ,{shellType::BULLET,"BULLET"} };

std::string getShellType(shellType& shellType) {
    return shellTypes[shellType];
}

void reserveProjectiles() {
    projectiles.reserve(20);
}
float calculatePenetration(float vel) {
    const float k = 0.0005f;
    return k * vel * vel;
}
void onHit(Projectile& p, int id, Health* health,EffectsContext& context,
    Sound& sound, Tank& player,bool hitGround) {

    const char* snd = "Explosion";
    sound.setSourcePosition(sound.sources[snd], p.pos);
    alSourceStop(sound.sources[snd]);
    alSourcePlay(sound.sources[snd]);

    if (!hitGround && health) {
        bool wasAlive = health->current > 0.0f;

        if (calculatePenetration(p.speed) >= health->armor) {
            health->current -= p.damage;

            if (wasAlive && health->current <= 0.0f) {
                player.addKill(1);

                health->destroyed = true;
                g_destroyText = "Target Destroyed";

                if (!p.isEnemy && p.type != ProjectileType::Bullet)
                    player.setScore(player.getScoreToCount());

                addToKillChat("Player",getRenderTypeString(renders[id].type),getShellType(p.selectedShellType),0,id);

                sound.setSourcePosition(sound.sources["Kill"], player.getCurrentPos());
                alSourcePlay(sound.sources["Kill"]);
            }
            else if (health->current > 0.0f) {
                g_destroyText = "Target hit";

                if (!p.isEnemy && p.type != ProjectileType::Bullet) 
                    player.setScore(player.getScoreToCount() / 2);
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

            context.explosions.push_back(new ExplosionEffect(p.pos, count, radius, height, 1.8f));
            context.smokes.push_back(new SmokeEffect(p.pos, 300, 3));
        }
    }

    p.alive = false;
}
void spawnShell(svbmath::Vec3 pos, float yawDeg, float pitchDeg, shellType _shellType,
        bool isEnemy) {
    Projectile p(_shellType);
    p.type = ProjectileType::Shell;

    p.selectedShellType = _shellType;

    p.pos = pos;
    p.speed = shellSpeeds[_shellType];

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
    p.pos = pos;
    p.speed = 120.0f;
    p.damage = 1;
    p.lifeTime = 4.0f;

    float yaw = yawDeg * 3.1415926f / 180.0f;

    p.velocity = { sin(yaw) * p.speed, 0.0f, cos(yaw) * p.speed };

    projectiles.push_back(p);
}
void update(float dt,Sound& sound, std::vector<Projectile>& artilleryProjectiles, 
    ECSCompenents& components,EffectsContext& context,Tank& player, CameraShake& shake) {

    for (auto& p : projectiles) {
        if (!p.alive) continue;

        p.update(dt, player);

        startAPS(p, context, sound, player);

        if (checkCollision(player.GetHullMax(), p.pos) && p.isEnemy) {
            player.getCurretHp() -= p.damage;

            shake.Start(1.5f,1.5f);

            onHit(p, 0, nullptr, context, sound, player, false);

            if (player.getCurretHp() <= 0) {
                addToKillChat("Tank", "Player", getShellType(p.selectedShellType), 0, 0);

                player.getDeath()++;
                player.getCurretHp() = player.getHp();
                player.respawn(player.getSelectedSpawn());
            }

            continue;
        }

        for (auto& [id, en] : components.enemyes) {

            if (!components.healths.contains(id)) continue;
            if (!components.bounds.contains(id)) continue;
            if (components.healths[id].destroyed && renders[id].type != RenderType::Apartment) continue;

            if (checkCollision(components.bounds[id], p.pos) && !p.isEnemy) {

                onHit(p,id, &components.healths[id], context, sound, player, false);
                break;
            }
        }
        if (p.alive && p.pos.y <= 0.0f && p.type != ProjectileType::Bullet) {
            onHit(p, 0, nullptr, context, sound, player, true);
        }
    }

    for (auto& p : artilleryProjectiles) {
        if (!p.alive) continue;

        bool exploded = false;

        for (auto& [id, en] : components.enemyes) {
            if (!components.healths.contains(id) || components.healths[id].destroyed) continue;
            if (!components.bounds.contains(id)) continue;

            if (checkCollision(components.bounds[id], p.pos) && calculatePenetration(p.speed)) {
                onHit(p, id, &components.healths[id], context, sound, player, false);
                exploded = true;

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

    std::erase_if(projectiles, [](const Projectile& p) { return !p.alive; });
}
void drawProjectiles() {
    for (auto& p : projectiles) {
        if (!p.alive) continue;

        glPushMatrix();
        glTranslatef(p.pos.x, p.pos.y, p.pos.z);

        if (p.type == ProjectileType::Shell) drawShell();
        else drawBullet();

        glPopMatrix();
    }
}