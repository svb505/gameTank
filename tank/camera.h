#pragma once
#include "tank.h"

class Tank;
class Camera {
public:
    bool zoomed = false;

    float fov = 70.0f;
    float aspect = 1600.0f / 1000.0f;
    float nearPlane = 0.1f;
    float farPlane = 300.0f;

    float top = tan(fov * 3.1415926f / 360.0f) * nearPlane; 
    float bottom = -top; 
    float right = top * aspect; 
    float left = -right; 
    float angle = 0.0f;

    float camDistance = 6.0f;
    float camHeight = 2.5f;
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    float cameraZ = 0.0f;

    float cameraYaw = 0.0f; 
    float cameraPitch = 10.0f;

    void setupCamera(Tank& playerTank, bool& aimMode);
};
	
