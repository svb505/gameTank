#include "camera.h"
#include <GL/glu.h>
#include "tank.h"
#include "cameraShake.h"
#include "variables.h"

void Camera::setupCamera(Tank& tank, CameraShake& shakeEffect,bool& aimMode) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Ограничение pitch
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -10.0f) cameraPitch = -10.0f;

    float yawRad = cameraYaw * 3.1415926f / 180.0f;
    float pitchRad = cameraPitch * 3.1415926f / 180.0f;

    svbmath::Vec3 offset = shakeEffect.GetOffset();

    if (!aimMode) {
        cameraPos.x = tank.pos.x - cos(pitchRad) * sin(yawRad) * camDistance;
        cameraPos.y = tank.pos.y + camHeight + sin(pitchRad) * camDistance;
        cameraPos.z = tank.pos.z + cos(pitchRad) * cos(yawRad) * camDistance;

        gluLookAt(cameraPos.x + offset.x,cameraPos.y + offset.y,cameraPos.z + offset.z,
            tank.pos.x, tank.pos.y + 1.0f, tank.pos.z,0,1,0);
    }
    else {
        float turretRad;

        if (!tank.turretLocked) turretRad = (tank.turretYaw - 90.0f) * 3.1415926f / 180.0f;
        else turretRad = (tank.bodyYaw + tank.turretYaw) * 3.1415926f / 180.0f;

        float forwardX = sin(turretRad);
        float forwardZ = cos(turretRad);

        cameraPos.x = tank.pos.x - forwardX * 0.5f;
        cameraPos.y = tank.pos.y + (tank.params.hullH + tank.params.turretY - tank.params.gunOffsetY);
        cameraPos.z = tank.pos.z - forwardZ * 0.5f;

        float gunPitchRad = tank.gunPitch * 3.1415926f / 180.0f;
        float lookX = tank.pos.x + forwardX;
        float lookY = cameraPos.y + sin(gunPitchRad);
        float lookZ = tank.pos.z + forwardZ;

        gluLookAt(cameraPos.x + offset.x, cameraPos.y + offset.y, cameraPos.z + offset.z,
            lookX, lookY, lookZ,
            0, 1, 0);
    }
}
svbmath::Vec3 Camera::returnForwardVector() {
    float radYaw = cameraYaw * 3.14159265f / 180.0f;
    float radPitch = angle * 3.14159265f / 180.0f;
    float fx = cos(radPitch) * sin(radYaw);
    float fy = sin(radPitch);
    float fz = -cos(radPitch) * cos(radYaw);

    return {fx,fy,fz};
}