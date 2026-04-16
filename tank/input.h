#pragma once
#include <GLFW/glfw3.h>
#include "projectile.h"
#include <unordered_map>
#include <vector>
#include "tank.h"
#include "enemyes.h"
#include "projectileSystem.h"
#include "camera.h"
#include "rangefinder.h"
#include "minimap.h"
#include "smokeGranade.h"

inline bool isDown(GLFWwindow* w, int key) {
    return glfwGetKey(w, key) == GLFW_PRESS;
}
inline bool isPressed(GLFWwindow* w, int key, bool& prev) {
    bool curr = glfwGetKey(w, key) == GLFW_PRESS;
    bool result = curr && !prev;
    prev = curr;
    return result;
}
struct ShellBind {
    int key;
    shellType type;
    float speed;
};
struct RayContext {
    Ray& debugRay;
    bool& drawDebugRay;

    int& lastHitID;
    float& lastHitDist;
};

void processTankInput(GLFWwindow* window, float dt, std::unordered_map<int, Entity>& enemyes,Tank& tank,Sound& sound,
    Camera& cam,RayContext& context,SmokeGranade& granades,bool& cursorVisibility) {
    static bool prevCtrl = false;
    static bool prevAlt = false;
    static bool prevShift = false;
    static bool prevG = false;
    static bool prevR = false;
    static bool prevFire = false;

    static std::vector<ShellBind> shellBinds = {
        {GLFW_KEY_1, shellType::APFSDS, 400},
        {GLFW_KEY_2, shellType::HE, 200},
        {GLFW_KEY_3, shellType::ATGM, 10},
        {GLFW_KEY_4, shellType::SMOKE, 200}
    };

    for (auto& s : shellBinds) {
        if (isDown(window, s.key)) {
            tank.selectedShell = s.type;
            tank.shellSpeed = s.speed;
            tank.finishReload = tank.reloadTime;
        }
    }

    if (isDown(window, GLFW_KEY_A)) { tank.bodyYaw += tank.rotateSpeed * dt; tank.moveSpeed *= tank.REDUCTION_COEF; }
    if (isDown(window, GLFW_KEY_D)) { tank.bodyYaw -= tank.rotateSpeed * dt; tank.moveSpeed *= tank.REDUCTION_COEF; }
    if (isDown(window, GLFW_KEY_W)) {
        tank.oldX = tank.x;
        tank.oldY = tank.y;
        tank.oldZ = tank.z;

        if (tank.moveSpeed <= tank.SPEED_LIMIT_FORWARD) tank.moveSpeed += tank.VELOCITY_COEF;
    }
    if (isDown(window, GLFW_KEY_S)) {
        tank.oldX = tank.x;
        tank.oldY = tank.y;
        tank.oldZ = tank.z;

        if (tank.moveSpeed >= tank.SPEED_LIMIT_BACK) tank.moveSpeed -= tank.VELOCITY_COEF / 2;
    }

    if (isDown(window, GLFW_KEY_UP) && isDown(window, GLFW_KEY_LEFT_CONTROL)) setHeight(getHeight() + step * dt * 60);
    if (isDown(window, GLFW_KEY_DOWN) && isDown(window, GLFW_KEY_LEFT_CONTROL)) setHeight(getHeight() - step * dt * 60);

    if (isDown(window, GLFW_KEY_ENTER)) {
        float yaw = tank.bodyYaw + tank.turretYaw;

        spawnBullet({ tank.x, tank.y + 1.0f, tank.z }, yaw);

        auto& src = sound.sources["MGun"];
        sound.setSourcePosition(src, tank.x, tank.y + 1.6f, tank.z);
        alSourceStop(src);
        alSourcePlay(src);
    }

    if (isPressed(window, GLFW_KEY_R, prevR)) {
        Ray ray;
        ray.origin = { tank.x, tank.y + 1.6f, tank.z };

        float yawRad = (tank.bodyYaw + tank.turretYaw) * 3.14159265f / 180.0f;
        float pitchRad = tank.gunPitch * 3.14159265f / 180.0f;

        ray.direction = { sin(yawRad) * cos(pitchRad), sin(pitchRad), cos(yawRad) * cos(pitchRad) };

        float maxDist = 300.0f;

        int hitID = -1;
        float hitDistance = Raycast(ray, enemyes, bounds, hitID, maxDist);

        context.debugRay = ray;
        context.drawDebugRay = true;

        if (hitID != -1) { context.lastHitID = hitID; context.lastHitDist = hitDistance; }
        else { context.lastHitID = -1; context.lastHitDist = 0.0f; }
    }

    if (isPressed(window, GLFW_KEY_G, prevG)) granades.strike();
    if (isPressed(window, GLFW_KEY_LEFT_ALT, prevAlt)) {
        cursorVisibility = !cursorVisibility;

        glfwSetInputMode(window, GLFW_CURSOR, cursorVisibility ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    if (isPressed(window, GLFW_KEY_LEFT_SHIFT, prevShift)) tank.aimMode = !tank.aimMode;

    if (isPressed(window, GLFW_KEY_SPACE, prevFire) && tank.finishReload <= 0.0f && tank.totalShells > 0) {
        float yaw = tank.turretLocked ? tank.bodyYaw + tank.turretYaw : tank.turretYaw - 90.0f;

        spawnShell({ tank.x, tank.y + 1.6f, tank.z }, yaw, tank.gunPitch, tank.selectedShell, tank.shellSpeed);

        auto& src = sound.sources["Shot"];
        sound.setSourcePosition(src, tank.x, tank.y + 1.6f, tank.z);
        alSourceStop(src);
        alSourcePlay(src);

        --tank.totalShells;
        tank.finishReload = tank.reloadTime;
    }

    if (isPressed(window, GLFW_KEY_LEFT_CONTROL, prevCtrl)) { cam.zoomed = !cam.zoomed; cam.fov = cam.zoomed ? 40.0f : 70.0f; }
}