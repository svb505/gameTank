#pragma once
#include <cmath>

enum class ProjectileType {
    Shell,
    Bullet
};
enum class shellType {
    APFSDS,
    HE,
    SMOKE
};

struct Projectile {
    ProjectileType type;
    shellType selectedShellType;

    float x, y, z;
    float vx, vy, vz;

    float gravity = 9.8f;
    float speed;
    int damage;

    float lifeTime = 7.0f; 
    bool alive = true;

    float delay = 5.0f; 
    bool active = false;

    void update(float dt) {
        vy -= gravity * dt;

        x += vx * dt;
        y += vy * dt;
        z += vz * dt;

        lifeTime -= dt;

        if (lifeTime <= 0.0f)
            alive = false;
    }
    

};
