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

    void spawnShell(float x, float y, float z,float yawDeg, float pitchDeg,shellType _shellType,int shellSpeed){
        Projectile p(_shellType);
        p.type = ProjectileType::Shell;

        p.selectedShellType = _shellType;
        
        p.x = x; p.y = y; p.z = z;
        p.speed = shellSpeed;

        float yaw = yawDeg * 3.1415926f / 180.0f;
        float pitch = pitchDeg * 3.1415926f / 180.0f;

        p.vx = sin(yaw) * cos(pitch) * p.speed;
        p.vy = -sin(pitch) * p.speed;
        p.vz = cos(yaw) * cos(pitch) * p.speed;

        projectiles.push_back(p);
    }
    void spawnBullet(float x, float y, float z,float yawDeg){
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
    void update(float dt,Sound& sound, std::unordered_map<int, Entity>& enemies,std::unordered_map<Entity, Health>& healths,
        std::unordered_map<Entity, Bounds>& bounds,std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes,ALuint explosionSource){
        
        for (auto& p : projectiles) {
            if (!p.alive) continue;
            
            p.update(dt);

            for (auto& [id, en] : enemies) {
                if (!healths.contains(id)) continue;
                if (healths[id].destroyed && renders[id].type != RenderType::Apartment) continue;
                if (!bounds.contains(id)) continue;

                if (checkCollision(bounds[id], p.x,p.y,p.z)) {
                    healths[id].current -= p.damage;
                    if (healths[id].current <= healths[id].max / 2) {
if (apartments.contains(id)) apartments[id].LOD = 2;
                    }

                    sound.setSourcePosition(sound.explosionSource, p.x, p.y, p.z);
                    alSourceStop(explosionSource);
                    alSourcePlay(explosionSource);

                    if (p.type != ProjectileType::Bullet) explosions.push_back(new ExplosionEffect(p.x, p.y, p.z));
                    if (p.selectedShellType == shellType::SMOKE) smokes.push_back(new SmokeEffect(p.x, p.y, p.z));

                    p.alive = false;
                    break;
                }
                if (p.alive && p.y <= 0.0f && p.type != ProjectileType::Bullet) {
                    onHit(p, en, healths[id], explosions, smokes, explosionSource, sound,false,p.selectedShellType == shellType::SMOKE);

                    if (p.selectedShellType == shellType::SMOKE) smokes.push_back(new SmokeEffect(p.x, p.y, p.z));

                    explosions.push_back(new ExplosionEffect(p.x, p.y, p.z));

                    p.alive = false;
                }
            }
            
        }
        std::erase_if(projectiles, [](const Projectile& p) { return !p.alive; });
    }
    void updateProjectiles(ProjectileSystem& projectileSystem) {
        for (auto& p : projectileSystem.projectiles) {
            if (!p.alive) continue;

            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);

            if (p.type == ProjectileType::Shell) drawShell();
            else drawBullet();

            glPopMatrix();
        }
    }
    void updateArtillery(std::vector<Projectile>& artilleryProjectiles, Sound& sound,
        std::unordered_map<int, Entity>& enemies, std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes, ALuint explosionSource) {
        for (auto& p : artilleryProjectiles) {
            if (!p.alive) continue;

            bool exploded = false;

            for (auto& [id, en] : enemies) {
                if (!healths.contains(id) || healths[id].destroyed) continue;
                if (!bounds.contains(id)) continue;

                if (checkCollision(bounds[id], p.x, p.y, p.z)) {
                    healths[id].current -= p.damage;

                    explosions.push_back(new ExplosionEffect(p.x, p.y, p.z,200));

                    p.alive = false;
                    exploded = true;
                    sound.setSourcePosition(sound.explosionSource, p.x, p.y, p.z);
                    alSourcePlay(explosionSource);
                    break;
                }
            }
            if (!exploded && p.y <= 0.0f) {
                explosions.push_back(new ExplosionEffect(p.x, p.y, p.z, 200));
                sound.setSourcePosition(sound.explosionSource, p.x, p.y, p.z);
                alSourcePlay(explosionSource);
                p.alive = false;
            }
        }
    }
private:
    void onHit(Projectile& p,Entity& en,Health& health,std::vector<ExplosionEffect*>& explosions,
        std::vector<SmokeEffect*>& smokes,ALuint explosionSource,Sound& sound,bool hitGround = false,bool smokeShell = false){
        
        sound.setSourcePosition(sound.explosionSource, p.x, p.y, p.z);
        alSourceStop(explosionSource);
        alSourcePlay(explosionSource);

        float x = p.x;
        float y = p.y;
        float z = p.z;

        if (!hitGround) {
            health.current -= p.damage;
            if (health.current <= 0) health.destroyed = true;
        }

        if (p.type == ProjectileType::Shell) {
            int count,radius = 0;
            float height = 0.0f;

            if (!smokeShell) {
                if (p.selectedShellType == shellType::APFSDS) { count = 300; radius = 4; height = 1.5f; }
                else { count = 500; radius = 6; height = 2.0f; }

                explosions.push_back(new ExplosionEffect(x, y, z, count, radius, height, 1.8f));
                smokes.push_back(new SmokeEffect(x, y, z, 300, 3));
            }
            else smokes.push_back(new SmokeEffect(x, y, z, 5500, 6,{1.0f,1.0f,1.0f,0.3f},3.0f,0.01f,6.0f));     
        } 
    }
};
