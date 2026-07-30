#pragma once
#include "rangefinder.h"

enum class DayTime {
	DAY, NIGHT
};

//Main
extern int ECRANW;
extern int ECRANH;
extern bool firstMouse;
extern bool cursorVisibility;
extern bool fpsLimit;
extern double lastX, lastY; //Cursor
extern float sensitivity; //Cursor
extern int waitingForBind; //Input
//Rangefinder
extern int lastHitID; 
extern float lastHitDist;
extern  Ray debugRay;
extern  bool drawDebugRay;

//Weather
extern DayTime dayTime;