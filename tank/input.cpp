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
#include <format>
#include "text.h"
#include "input.h"
#include "variables.h"
#include "json_parser.h"

std::vector<KeyBind> binds = { {"Fire",GLFW_KEY_SPACE},{"Forward",GLFW_KEY_W},
                              {"Back",GLFW_KEY_S},{"Left",GLFW_KEY_A},
                              {"Right",GLFW_KEY_D},{"Rangefinder",GLFW_KEY_R},
                              {"Smoke",GLFW_KEY_G},{"MGun",GLFW_KEY_ENTER},
                              {"AimMode",GLFW_KEY_LEFT_SHIFT},
                              {"CursorVisibility",GLFW_KEY_LEFT_ALT},
                              {"Zoom",GLFW_KEY_LEFT_CONTROL},
};//Base values

std::vector<std::string> allKeys = { "Fire","Forward","Back","Left","Right",
                                    "Rangefinder","Smoke","MGun","AimMode","CursorVisibility",
                                    "Zoom"};

std::vector<BannedKey> bannedKeysForChanging = { {"Fire","SPACE"},{"MGun","ENTER"},
    {"AimMode","SHIFT"},{"Zoom","CTRL"},{"CursorVisibility","ALT" } };


inline bool isDown(GLFWwindow* w, int key) {
    return glfwGetKey(w, key) == GLFW_PRESS;
}
inline bool isPressed(GLFWwindow* w, int key, bool& prev) {
    bool curr = glfwGetKey(w, key) == GLFW_PRESS;
    bool result = curr && !prev;
    prev = curr;
    return result;
}
int getKey(const std::string& action){
    for (const auto& b : binds)
        if (b.action == action)
            return b.key;

    return GLFW_KEY_UNKNOWN;
}
const char* getKeyName(int key) {
    for (auto& b : bannedKeysForChanging) {
        if (getKey(b.action) == key) return b.keyName.c_str();
    }

    return glfwGetKeyName(key, 0);
}
std::string getAction(const int key) {
    for (const auto& b : binds)
        if (b.key == key) return b.action;

    return "UNKNOWN";
}

void setupBinds() {
    bool banned = false;

    for (size_t i = 0; i < allKeys.size(); i++) {
        for (auto& b : bannedKeysForChanging) { if (b.action == allKeys[i]) 
            banned = true; }

        if (banned) { banned = false;  continue; }

        binds[i].key = getFloatFromJson(allKeys[i],"keyBinds.json");
    }
}

void processTankInput(GLFWwindow* window, float dt, std::unordered_map<int, Entity>& enemyes, Tank& tank, Sound& sound,
    Camera& cam, RayContext& context, SmokeGranade& granades, CameraShake& shake) {
    static bool prevCtrl = false;
    static bool prevAlt = false;
    static bool prevShift = false;
    static bool prevG = false;
    static bool prevR = false;
    static bool prevFire = false;

    static std::vector<ShellBind> shellBinds = {
        {GLFW_KEY_1, shellType::APFSDS, 400},
        {GLFW_KEY_2, shellType::HE, 100},
        {GLFW_KEY_3, shellType::ATGM, 10},
        {GLFW_KEY_4, shellType::SMOKE, 200}
    };

    for (auto& s : shellBinds) {
        if (isDown(window, s.key)) {
            tank.getSelectedShell() = s.type;
            tank.getBaseShellSpeed() = s.speed;
            tank.getFinishReload() = tank.getReloadTime();
        }
    }

    
    if (isDown(window, getKey("Left"))) {
        tank.getBodyYaw() += tank.getRotateSpeed() * dt;
        tank.getMoveSpeed() *= tank.getReductionCoef();
    }
    if (isDown(window, getKey("Right"))) {
        tank.getBodyYaw() -= tank.getRotateSpeed() * dt;
        tank.getMoveSpeed() *= tank.getReductionCoef();
    }
    if (isDown(window, getKey("Forward"))) {
        tank.getOldPos() = tank.getCurrentPos();

        if (tank.getMoveSpeed() <= tank.getSpeedLimitForward()) tank.getMoveSpeed()
            += tank.getVelocityCoef();
    }
    if (isDown(window, getKey("Back"))) {
        tank.getOldPos() = tank.getCurrentPos();

        if (tank.getMoveSpeed() >= tank.getSpeedLimitBack()) tank.getMoveSpeed() -= tank.getVelocityCoef() / 2;
    }
    if (isDown(window, getKey("MGun"))) {
        float yaw = tank.getBodyYaw() + tank.getTurretYaw();
        auto& src = sound.sources["MGun"];
        svbmath::Vec3 pos = tank.getCurrentPos();

        ALint state;

        alGetSourcei(src, AL_SOURCE_STATE, &state);
        spawnBullet({ pos.x, pos.y + 1.0f, pos.z }, yaw);

        if (!sound.mgunPlayed) {
            sound.setSourcePosition(src, tank.getCurrentPos());
            alSourceStop(src);
            alSourcePlay(src);

            sound.mgunPlayed = true;
        }

        if (state == AL_STOPPED) sound.mgunPlayed = false;


    }
    if (isPressed(window, getKey("Rangefinder"), prevR)) {
        Ray ray;
        ray.origin = { tank.getCurrentPos().x, tank.getCurrentPos().y + 1.6f, tank.getCurrentPos().z };

        float yawRad = (tank.getBodyYaw() + tank.getTurretYaw()) * 3.14159265f / 180.0f;
        float pitchRad = tank.getGunPitch() * 3.14159265f / 180.0f;

        ray.direction = { sin(yawRad) * cos(pitchRad), sin(pitchRad), cos(yawRad) * cos(pitchRad) };

        float maxDist = 300.0f;

        int hitID = -1;
        float hitDistance = Raycast(ray, enemyes, bounds, hitID, maxDist);

        context.debugRay = ray;
        context.drawDebugRay = true;

        if (hitID != -1) { context.lastHitID = hitID; context.lastHitDist = hitDistance; }
        else { context.lastHitID = -1; context.lastHitDist = 0.0f; }
    }
    if (isPressed(window, getKey("Smoke"), prevG)) granades.strike();
    if (isPressed(window, getKey("CursorVisibility"), prevAlt)) {
        cursorVisibility = !cursorVisibility;

        glfwSetInputMode(window, GLFW_CURSOR, cursorVisibility ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    if (isPressed(window, getKey("AimMode"), prevShift)) tank.getAimMode() = !tank.getAimMode();
    if (isPressed(window, getKey("Fire"), prevFire) && tank.getFinishReload() <= 0.0f &&
        tank.getTotalShells() > 0) {

        TankParams params = tank.getParams();
        svbmath::Vec3 pos = tank.getCurrentPos();

        float yaw = tank.getTurretLocked() ? tank.getBodyYaw() + tank.getTurretYaw() :
            tank.getTurretYaw() - 90.0f;

        float posY = pos.y + params.hullH + params.turretY - params.gunOffsetY;

        shake.Start(0.2f, 0.5f);

        spawnShell({ pos.x, posY, pos.z }, yaw, tank.getGunPitch(), tank.getSelectedShell());

        sound.setSourcePosition(sound.sources["Shot"], tank.getCurrentPos());
        alSourceStop(sound.sources["Shot"]);
        alSourcePlay(sound.sources["Shot"]);

        --tank.getTotalShells();
        tank.getFinishReload() = tank.getReloadTime();
    }

    if (isPressed(window, getKey("Zoom"), prevCtrl)) {
        cam.zoomed = !cam.zoomed;
        cam.fov = cam.zoomed ? 40.0f : 70.0f;
    }
    if (isDown(window, GLFW_KEY_UP) && isDown(window, GLFW_KEY_LEFT_CONTROL)) setHeight(getHeight() + step * dt * 60);
    if (isDown(window, GLFW_KEY_DOWN) && isDown(window, GLFW_KEY_LEFT_CONTROL)) setHeight(getHeight() - step * dt * 60);
    
}