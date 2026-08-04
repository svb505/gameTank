#pragma once
#include <vector>
#include <string>

class Tank;
class ProjectileSystem;
class Camera;
enum class Type;

struct EffectsContex;

extern float height;
extern int step;
extern bool badges;

void setHeight(float h);
float getHeight();
void drawMiniMap(int ECRANW, int ECRANH, Tank& playerTank, EffectsContext& context, Camera& cam,
    Type weather, bool badges, double dt);

    