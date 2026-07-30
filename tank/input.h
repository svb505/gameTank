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

struct KeyBind {
    std::string action;
    int key;
};

struct BannedKey {
    std::string action;
    std::string keyName;
};

extern std::vector<KeyBind> binds;
extern std::vector<BannedKey> bannedKeysForChanging;
extern std::vector<std::string> allKeys;

inline bool isDown(GLFWwindow* w, int key);
inline bool isPressed(GLFWwindow* w, int key, bool& prev);
int getKey(const std::string& action);
const char* getKeyName(int key);
std::string getAction(const int key);

void setupBinds();

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

void processTankInput(GLFWwindow* window, float dt, std::unordered_map<int, Entity>& enemyes, Tank& tank, Sound& sound,
    Camera& cam, RayContext& context, SmokeGranade& granades, CameraShake& shake);