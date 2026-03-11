#pragma once
#include <vector>

class Tank;
class ProjectileSystem;
class ExplosionEffect;
class SmokeEffect;
class Camera;

class MiniMap {
private:
    float height = 80.0f;
public:
    int step = 3;

    void setHeight(float h) { height = h; }
    float getHeight() { return height; }
    void draw(int ECRANW, int ECRANH, Tank& playerTank, ProjectileSystem& projectileSystem,
        std::vector<ExplosionEffect*>& explosions, std::vector<SmokeEffect*>& smokes, Camera& cam,
        float dt);
};

    