#pragma once
#include <cstdint>

struct Bounds {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
};
struct checkCol {
    bool checked;
    uint32_t id;
};

bool checkCollision(const Bounds& target, float x, float y, float z);