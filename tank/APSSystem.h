#pragma once
#include "effects.h"
#include "sounds.h"
#include "svbmath.h"
#include "tank.h"

extern float MAX_SHL_SPD;
extern float RADIUS;
extern int counter_munition;

struct Projectile;

bool shellInRadius(const svbmath::Vec3& shellPos,const svbmath::Vec3 tankPos,float radius);
void startAPS(Projectile& p, EffectsContext& context, Sound& sound,Tank& tank);