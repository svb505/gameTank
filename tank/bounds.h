#pragma once

struct Bounds {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
};

bool checkCollision(const Bounds& target, float x, float y, float z);