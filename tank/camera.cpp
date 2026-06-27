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
        cameraPos.x = tank.getCurrentPos().x - cos(pitchRad) * sin(yawRad) * camDistance;
        cameraPos.y = tank.getCurrentPos().y + camHeight + sin(pitchRad) * camDistance;
        cameraPos.z = tank.getCurrentPos().z + cos(pitchRad) * cos(yawRad) * camDistance;

        gluLookAt(cameraPos.x + offset.x,cameraPos.y + offset.y,cameraPos.z + offset.z,
            tank.getCurrentPos().x, tank.getCurrentPos().y + 1.0f, tank.getCurrentPos().z,0,1,0);
    }
    else {
        float turretRad;

        if (!tank.getTurretLocked()) turretRad = (tank.getTurretYaw() - 90.0f) * 3.1415926f / 180.0f;
        else turretRad = (tank.getBodyYaw() + tank.getTurretYaw()) * 3.1415926f / 180.0f;

        float forwardX = sin(turretRad);
        float forwardZ = cos(turretRad);

        cameraPos.x = tank.getCurrentPos().x - forwardX * 0.5f;
        cameraPos.y = tank.getCurrentPos().y + (tank.getParams().hullH + tank.getParams().turretY - tank.getParams().gunOffsetY);
        cameraPos.z = tank.getCurrentPos().z - forwardZ * 0.5f;

        float gunPitchRad = tank.getGunPitch() * 3.1415926f / 180.0f;
        float lookX = tank.getCurrentPos().x + forwardX;
        float lookY = cameraPos.y + sin(gunPitchRad);
        float lookZ = tank.getCurrentPos().z + forwardZ;

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