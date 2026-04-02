#include "camera.h"
#include <GL/glu.h>
#include "tank.h"

void Camera::setupCamera(Tank& playerTank, bool& aimMode) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Ограничение pitch
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -10.0f) cameraPitch = -10.0f;

    float yawRad = cameraYaw * 3.1415926f / 180.0f;
    float pitchRad = cameraPitch * 3.1415926f / 180.0f;

    if (!aimMode) {
        cameraX = playerTank.x - cos(pitchRad) * sin(yawRad) * camDistance;
        cameraY = playerTank.y + camHeight + sin(pitchRad) * camDistance;
        cameraZ = playerTank.z + cos(pitchRad) * cos(yawRad) * camDistance;

        gluLookAt(cameraX, cameraY, cameraZ,
            playerTank.x, playerTank.y + 1.0f, playerTank.z,
            0, 1, 0);
    }
    else {
        float turretRad = (playerTank.bodyYaw + playerTank.turretYaw) * 3.1415926f / 180.0f;
        float forwardX = sin(turretRad);
        float forwardZ = cos(turretRad);

        cameraX = playerTank.x - forwardX * 0.5f;
        cameraY = playerTank.y + 1.5f;
        cameraZ = playerTank.z - forwardZ * 0.5f;

        float gunPitchRad = playerTank.gunPitch * 3.1415926f / 180.0f;
        float lookX = playerTank.x + forwardX;
        float lookY = cameraY + sin(gunPitchRad);
        float lookZ = playerTank.z + forwardZ;

        gluLookAt(cameraX, cameraY, cameraZ,
            lookX, lookY, lookZ,
            0, 1, 0);
    }
}