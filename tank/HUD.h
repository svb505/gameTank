#pragma once
#include <vector>

class Projectile;
class Tank;
class HUD {
public:
	void drawHud(int screenWidth, int screenHeight, Tank& tank,const int count,float fps);
	void Draw3DAim(Tank& tank);
};
