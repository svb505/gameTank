#pragma once
#include "effects.h"
#include "sounds.h"
#include "svbmath.h"
#include "tank.h"

extern float MAX_SHL_SPD;
extern float RADIUS;
extern int counter_munition;

struct Projectile;

void startAPS(Projectile& p, EffectsContext& context, Sound& sound,Tank& tank);