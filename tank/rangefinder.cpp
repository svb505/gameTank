#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "svbmath.h"
#include "rangefinder.h"
#include <algorithm>
#include "bounds.h"

float Raycast(const Ray& ray,const std::unordered_map<int, Entity>& enemies,const std::unordered_map<Entity, Bounds>& bounds,
    int& hitEnemyID, float maxDistance){
    hitEnemyID = -1;

    svbmath::Vec3 dir = ray.direction;
    float len = svbmath::Length(dir);

    if (len < 1e-6f) return maxDistance;

    dir = dir / len;

    float nearestDist = maxDistance;
    svbmath::Vec3 hitPos;

    for (const auto& [id, entity] : enemies) {
        auto it = bounds.find(entity);
        if (it == bounds.end()) continue;

        const Bounds& b = it->second;

        float tNear, tFar;
        if (RayIntersectsAABB(ray.origin, dir, b, tNear, tFar)) {
            if (tNear < 0.0f) tNear = tFar;
            if (tNear < nearestDist) {
                nearestDist = tNear;
                hitEnemyID = id;
                hitPos = ray.origin + dir * tNear; 
            }
        }
    }

    return nearestDist;
}
