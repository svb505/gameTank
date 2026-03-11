#pragma once
#include "tank.h"

class Tank;
class Camera{
public:
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

    void setupCamera(Tank& playerTank,bool& aimMode){
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float camRad = cameraYaw * 3.1415926f / 180.0f;

        if (!aimMode) {
            cameraX = playerTank.x - sin(camRad) * camDistance;
            cameraY = playerTank.y + camHeight;
            cameraZ = playerTank.z + cos(camRad) * camDistance;

            gluLookAt(cameraX, cameraY, cameraZ,playerTank.x, playerTank.y + 1.0f, playerTank.z,0, 1, 0);
        }
        else {
            float turretRad = (playerTank.bodyYaw + playerTank.turretYaw) * 3.1415926f / 180.0f;

            float forwardX = sin(turretRad);
            float forwardZ = cos(turretRad);

            cameraX = playerTank.x - forwardX * 0.5f;
            cameraY = playerTank.y + 1.5f;
            cameraZ = playerTank.z - forwardZ * 0.5f;

            float pitchRad = playerTank.gunPitch * 3.1415926f / 180.0f;
            float lookX = playerTank.x + forwardX;
            float lookY = cameraY + sin(pitchRad);
            float lookZ = playerTank.z + forwardZ;

            gluLookAt(cameraX, cameraY, cameraZ,lookX, lookY, lookZ,0, 1, 0);
        }
    }
};
	
