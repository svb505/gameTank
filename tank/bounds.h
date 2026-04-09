#pragma once
#include <cstdint>
#include "svbmath.h"

struct Bounds {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
};
struct checkCol {
    bool checked;
    uint32_t id;
};

bool RayIntersectsAABB(const svbmath::Vec3& rayOrigin, const svbmath::Vec3& rayDir,
    const Bounds& b, float& tNear, float& tFar);
bool checkCollision(const Bounds& target, float x, float y, float z);