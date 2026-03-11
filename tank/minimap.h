#pragma once
#include <vector>

class Tank;
class ProjectileSystem;
class ExplosionEffect;
class SmokeEffect;
class Camera;

void drawMiniMap(int ECRANW, int ECRANH, Tank& playerTank, ProjectileSystem& projectileSystem,
    std::vector<ExplosionEffect*>& explosions, std::vector<SmokeEffect*>& smokes, Camera& cam,
    float dt);
    