#pragma once
#include <vector>

struct CameraParams;
class Tank;

void Draw2DAim(Tank& tank, CameraParams& cam);
void drawHUD(int WW,int WH,float lastHit);