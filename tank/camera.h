#pragma once
#include "tank.h"
#include "svbmath.h"
#include "cameraShake.h"

struct CameraParams {
    float baseFov = 70.0f;
    float fov = baseFov;
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

    float cameraYaw = 0.0f;
    float cameraPitch = 10.0f;

    bool zoomed = false;
};

class Tank;
class Camera {
private:
    CameraParams camParams;

    svbmath::Vec3 cameraPos = { 0.0f,0.0f,0.0f };
public:
    CameraParams& getCamParams();
    svbmath::Vec3& getCamPos();

    void setupCamera(Tank& playerTank, CameraShake& shakeEffect,bool& aimMode);
    svbmath::Vec3 returnForwardVector();
};
	
