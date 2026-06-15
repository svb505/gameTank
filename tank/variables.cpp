#include "variables.h"
#include "rangefinder.h"

int ECRANW = 1600; 
int ECRANH = 1000;

int lastHitID = -1;
float lastHitDist = 0.0f;

bool firstMouse = true;
bool cursorVisibility = false;
bool fpsLimit = false;

Ray debugRay;
bool drawDebugRay = false;

double lastX = 800.0 / 2, lastY = 600.0 / 2;
float sensitivity = 0.1f;