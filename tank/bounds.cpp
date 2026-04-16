#include "bounds.h"
#include "svbmath.h"
#include <algorithm>

bool checkCollision(const Bounds& target, svbmath::Vec3& pos) {
    return pos.x >= target.minX && pos.x <= target.maxX &&
        pos.y >= target.minY && pos.y <= target.maxY &&
        pos.z >= target.minZ && pos.z <= target.maxZ;
}
bool RayIntersectsAABB(const svbmath::Vec3& rayOrigin, const svbmath::Vec3& rayDir,
    const Bounds& b, float& tNear, float& tFar)
{
    tNear = -FLT_MAX;
    tFar = FLT_MAX;

    // Для каждой оси
    for (int i = 0; i < 3; ++i) {
        float origin = (i == 0 ? rayOrigin.x : (i == 1 ? rayOrigin.y : rayOrigin.z));
        float dir = (i == 0 ? rayDir.x : (i == 1 ? rayDir.y : rayDir.z));
        float minB = (i == 0 ? b.minX : (i == 1 ? b.minY : b.minZ));
        float maxB = (i == 0 ? b.maxX : (i == 1 ? b.maxY : b.maxZ));

        if (fabs(dir) < 1e-6f) { 
            if (origin < minB || origin > maxB)
                return false;
        }
        else {
            float t1 = (minB - origin) / dir;
            float t2 = (maxB - origin) / dir;
            if (t1 > t2) std::swap(t1, t2);
            tNear = std::max(tNear, t1);
            tFar = std::min(tFar, t2);
            if (tNear > tFar) return false;
            if (tFar < 0) return false; 
        }
    }
    return true;
}