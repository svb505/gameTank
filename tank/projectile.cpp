#include <cmath>
#include "projectile.h"

void Projectile::update(float dt) {
    vy -= gravity * dt;

    x += vx * redution_coef * dt;
    y += vy * dt;
    z += vz * redution_coef * dt;

    lifeTime -= dt;

    if (lifeTime <= 0.0f)
        alive = false;
}
