#pragma once
#include <cmath>

enum class ProjectileType {
    Shell,
    Bullet
};
enum class shellType {
    APFSDS,
    HE,
    SMOKE,
    BULLET
};

struct Projectile {
    ProjectileType type;
    shellType selectedShellType;

    float x, y, z;
    float vx, vy, vz;

    float gravity = 9.8f;
    float speed;
    int damage = 100;

    float lifeTime = 7.0f; 
    bool alive = true;

    float delay = 5.0f; 
    bool active = false;

    bool isEnemy = false;

    float redution_coef = 0.99f;

    Projectile(shellType shell) : selectedShellType(shell) {
        damage = (selectedShellType == shellType::APFSDS) ? 100 : 40;
    }
    void update(float dt);
};
