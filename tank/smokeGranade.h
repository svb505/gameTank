#pragma once
#include "svbmath.h"
#include <vector>

class Tank;
struct Position {
	svbmath::Vec3 pos;
	float speed = 15.0f;
	float vx, vy, vz;
	float gravity = 9.8f;
	int count;
	float angle = 45.0f;
	bool strike = false;
};

class SmokeGranade {
private:
	int maxCount = 6;
	
public:
	int currentGranade = 0;
	std::vector<Position> granades;

	void drawAll(Tank& tank);
	void draw(float x, float y, float z, float angle, float yaw);
	void spawn(Tank& tank);
	void update(float dt, std::vector<SmokeEffect*>& smokes, Tank& tank);
	void strike();
};
