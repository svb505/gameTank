#include "craters.h"

std::vector<Crater> craters = {};

void addCrater(float x, float z) { craters.push_back({ x, z, 2.0f, 2.0f }); }