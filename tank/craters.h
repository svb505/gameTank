#pragma once
#include <vector>

struct Crater {
    float x, z;
    float radius;
    float depth;
};

extern std::vector<Crater> craters;

void addCrater(float x, float z);