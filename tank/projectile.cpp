#include <cmath>
#include "projectile.h"

void Projectile::update(float dt) {
    vy -= gravity * dt;

    x += vx * dt;
    y += vy * dt;
    z += vz * dt;

    lifeTime -= dt;

    if (lifeTime <= 0.0f)
        alive = false;
}
