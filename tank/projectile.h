#pragma once
#include <cmath>
#include "svbmath.h"

class Tank;

enum class ProjectileType {
    Shell,
    Bullet
};
enum class shellType {
    APFSDS,
    HE,
    SMOKE,
    ATGM,
    BULLET
};

struct Projectile {
    ProjectileType type;
    shellType selectedShellType;

    svbmath::Vec3 dir;
    svbmath::Vec3 velocity;
    svbmath::Vec3 pos;

    float gravity = 9.8f;
    float speed;
    int damage = 100;

    float lifeTime = 7.0f; 
    bool alive = true;

    float delay = 5.0f; 
    bool active = false;

    bool isEnemy = false;

    float redution_coef = 0.99f;
    float turnSpeed;

    Projectile(shellType shell) : selectedShellType(shell) {
        damage = (selectedShellType == shellType::APFSDS) ? 100 : 40;
    }
    void update(float dt, Tank& tank);
};
