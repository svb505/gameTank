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
#include <unordered_map>

std::unordered_map<int, std::string> keyNames = {
    // Letters
    {GLFW_KEY_A, "A"},
    {GLFW_KEY_B, "B"},
    {GLFW_KEY_C, "C"},
    {GLFW_KEY_D, "D"},
    {GLFW_KEY_E, "E"},
    {GLFW_KEY_F, "F"},
    {GLFW_KEY_G, "G"},
    {GLFW_KEY_H, "H"},
    {GLFW_KEY_I, "I"},
    {GLFW_KEY_J, "J"},
    {GLFW_KEY_K, "K"},
    {GLFW_KEY_L, "L"},
    {GLFW_KEY_M, "M"},
    {GLFW_KEY_N, "N"},
    {GLFW_KEY_O, "O"},
    {GLFW_KEY_P, "P"},
    {GLFW_KEY_Q, "Q"},
    {GLFW_KEY_R, "R"},
    {GLFW_KEY_S, "S"},
    {GLFW_KEY_T, "T"},
    {GLFW_KEY_U, "U"},
    {GLFW_KEY_V, "V"},
    {GLFW_KEY_W, "W"},
    {GLFW_KEY_X, "X"},
    {GLFW_KEY_Y, "Y"},
    {GLFW_KEY_Z, "Z"},

    // Numbers
    {GLFW_KEY_0, "0"},
    {GLFW_KEY_1, "1"},
    {GLFW_KEY_2, "2"},
    {GLFW_KEY_3, "3"},
    {GLFW_KEY_4, "4"},
    {GLFW_KEY_5, "5"},
    {GLFW_KEY_6, "6"},
    {GLFW_KEY_7, "7"},
    {GLFW_KEY_8, "8"},
    {GLFW_KEY_9, "9"},

    // Main keys
    {GLFW_KEY_SPACE, "Space"},
    {GLFW_KEY_ENTER, "Enter"},
    {GLFW_KEY_TAB, "Tab"},
    {GLFW_KEY_BACKSPACE, "Backspace"},
    {GLFW_KEY_ESCAPE, "Escape"},
    {GLFW_KEY_INSERT, "Insert"},
    {GLFW_KEY_DELETE, "Delete"},
    {GLFW_KEY_HOME, "Home"},
    {GLFW_KEY_END, "End"},
    {GLFW_KEY_PAGE_UP, "Page Up"},
    {GLFW_KEY_PAGE_DOWN, "Page Down"},

    // Arrows
    {GLFW_KEY_UP, "Arrow Up"},
    {GLFW_KEY_DOWN, "Arrow Down"},
    {GLFW_KEY_LEFT, "Arrow Left"},
    {GLFW_KEY_RIGHT, "Arrow Right"},

    // Modificators
    {GLFW_KEY_LEFT_SHIFT, "Left Shift"},
    {GLFW_KEY_RIGHT_SHIFT, "Right Shift"},
    {GLFW_KEY_LEFT_CONTROL, "Left Ctrl"},
    {GLFW_KEY_RIGHT_CONTROL, "Right Ctrl"},
    {GLFW_KEY_LEFT_ALT, "Left Alt"},
    {GLFW_KEY_RIGHT_ALT, "Right Alt"},
    {GLFW_KEY_LEFT_SUPER, "Left Super"},
    {GLFW_KEY_RIGHT_SUPER, "Right Super"},

    // F-keys
    {GLFW_KEY_F1, "F1"},
    {GLFW_KEY_F2, "F2"},
    {GLFW_KEY_F3, "F3"},
    {GLFW_KEY_F4, "F4"},
    {GLFW_KEY_F5, "F5"},
    {GLFW_KEY_F6, "F6"},
    {GLFW_KEY_F7, "F7"},
    {GLFW_KEY_F8, "F8"},
    {GLFW_KEY_F9, "F9"},
    {GLFW_KEY_F10, "F10"},
    {GLFW_KEY_F11, "F11"},
    {GLFW_KEY_F12, "F12"},

    // NumPad
    {GLFW_KEY_KP_0, "Num 0"},
    {GLFW_KEY_KP_1, "Num 1"},
    {GLFW_KEY_KP_2, "Num 2"},
    {GLFW_KEY_KP_3, "Num 3"},
    {GLFW_KEY_KP_4, "Num 4"},
    {GLFW_KEY_KP_5, "Num 5"},
    {GLFW_KEY_KP_6, "Num 6"},
    {GLFW_KEY_KP_7, "Num 7"},
    {GLFW_KEY_KP_8, "Num 8"},
    {GLFW_KEY_KP_9, "Num 9"},
    {GLFW_KEY_KP_ENTER, "Num Enter"},
    {GLFW_KEY_KP_ADD, "Num +"},
    {GLFW_KEY_KP_SUBTRACT, "Num -"},
    {GLFW_KEY_KP_MULTIPLY, "Num *"},
    {GLFW_KEY_KP_DIVIDE, "Num /"},

    // Symbols
    {GLFW_KEY_MINUS, "-"},
    {GLFW_KEY_EQUAL, "="},
    {GLFW_KEY_LEFT_BRACKET, "["},
    {GLFW_KEY_RIGHT_BRACKET, "]"},
    {GLFW_KEY_SEMICOLON, ";"},
    {GLFW_KEY_APOSTROPHE, "'"},
    {GLFW_KEY_COMMA, ","},
    {GLFW_KEY_PERIOD, "."},
    {GLFW_KEY_SLASH, "/"},
    {GLFW_KEY_BACKSLASH, "\\"},
    {GLFW_KEY_GRAVE_ACCENT, "`"}
};

std::vector<KeyBind> binds = { {"Fire",GLFW_KEY_SPACE},{"Forward",GLFW_KEY_W},
                              {"Back",GLFW_KEY_S},{"Left",GLFW_KEY_A},
                              {"Right",GLFW_KEY_D},{"Rangefinder",GLFW_KEY_R},
                              {"Smoke",GLFW_KEY_G},{"MGun",GLFW_KEY_ENTER},
                              {"AimMode",GLFW_KEY_LEFT_SHIFT},
                              {"CursorVisibility",GLFW_KEY_LEFT_ALT},
                              {"Zoom",GLFW_KEY_LEFT_CONTROL},
};//Base values

std::vector<std::string> allBindedKeys = { "Fire","Forward","Back","Left","Right",
                                    "Rangefinder","Smoke","MGun","AimMode","CursorVisibility",
                                    "Zoom"};

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

std::string getKeyName(const int key) {
    for (auto& k : keyNames) {
        if (k.first == key) return k.second;
    }

    return "Unknown";
}

std::string getAction(const int key) {
    for (const auto& b : binds)
        if (b.key == key) return b.action;

    return "UNKNOWN";
}

void setupBinds() {
    for (size_t i = 0; i < allBindedKeys.size(); i++) {
        binds[i].key = getFloatFromJson(allBindedKeys[i],"keyBinds.json");
    }
}

void processTankInput(GLFWwindow* window, double dt, std::unordered_map<int, Entity>& enemyes, Tank& tank, Sound& sound,
    CameraParams& cam, RayContext& context, SmokeGranade& granades, CameraShake& shake) {
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
            sound.playSound(src, tank.getCurrentPos());

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

        sound.playSound(sound.sources["Shot"], tank.getCurrentPos());

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