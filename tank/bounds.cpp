#include "bounds.h"

bool checkCollision(const Bounds& target, float x, float y, float z) {
    return x >= target.minX && x <= target.maxX &&
        y >= target.minY && y <= target.maxY &&
        z >= target.minZ && z <= target.maxZ;
}