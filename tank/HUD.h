#pragma once
#include <vector>

class Camera;
class Tank;

void Draw2DAim(Tank& tank,Camera& cam);
void drawHUD(int WW,int WH,float lastHit);