#pragma once
#include "svbmath.h"
#include <vector>
#include <unordered_map>
#include "enemyes.h"

struct Ray {
    svbmath::Vec3 origin;
    svbmath::Vec3 direction;
};

float Raycast(const Ray& ray,
    const std::unordered_map<int, Entity>& enemies,
    const std::unordered_map<Entity, Bounds>& bounds,
    int& hitEnemyID,
    float maxDistance);
