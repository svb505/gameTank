#pragma once
#include <vector>
#include <string>

class Tank;
class ProjectileSystem;
struct EffectsContex;
class Camera;

extern float height;
extern int step;

void setHeight(float h);
float getHeight();
void drawMiniMap(int ECRANW, int ECRANH, Tank& playerTank, EffectsContext& context, Camera& cam, std::string weather, 
    bool badges, float dt);

    