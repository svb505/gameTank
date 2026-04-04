#include <cmath>
#include "projectile.h"
#include "tank.h"
#include "svbmath.h"

void Projectile::update(float dt,Tank& tank) {
    if (selectedShellType == shellType::ATGM) {
        float yaw = (tank.turretYaw + 90.0f) * 3.1415926f / 180.0f;
        float pitch = tank.gunPitch * 3.1415926f / 180.0f;

        svbmath::Vec3 targetDir{
            -sinf(yaw) * cosf(pitch),
            -sinf(pitch),
            cosf(yaw) * cosf(pitch)
        };

        float len = sqrtf(targetDir.x * targetDir.x + targetDir.y * targetDir.y + targetDir.z * targetDir.z);
        if (len > 0.0001f) {
            targetDir.x /= len;
            targetDir.y /= len;
            targetDir.z /= len;
        }

        float turn = turnSpeed * dt * 0.2f;

        dir.x += (targetDir.x - dir.x) * turn;
        dir.y += (targetDir.y - dir.y) * turn;
        dir.z += (targetDir.z - dir.z) * turn;

        float lenDir = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (lenDir > 0.0001f) {
            dir.x /= lenDir;
            dir.y /= lenDir;
            dir.z /= lenDir;
        }

        x += dir.x * speed * dt;
        y += dir.y * speed * dt;
        z += dir.z * speed * dt;
    }
    else {
        vy -= gravity * dt;

        x += vx * redution_coef * dt;
        y += vy * dt;
        z += vz * redution_coef * dt;
    }
    

    lifeTime -= dt;

    if (lifeTime <= 0.0f)
        alive = false;
}
