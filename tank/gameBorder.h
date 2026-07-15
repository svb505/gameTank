#pragma once
#include "svbmath.h"
#include "tank.h"

extern svbmath::Vec3 min;
extern svbmath::Vec3 max;

extern float timer;

void checkBorders(Tank& tank,float dt, int ScrH, int ScrW);