#pragma once
#include "svbmath.h"
#include <vector>

class Tank;
class Sound;

struct Position {
	svbmath::Vec3 pos;
	float speed = 15.0f;
	svbmath::Vec3 velocity;
	float gravity = 9.8f;
	int count;
	float angle = 45.0f;
	bool strike = false;
};

class SmokeGranade {	
public:
	int maxCount = 6;
	int currentGranade = 0;
	std::vector<Position> granades;

	void drawAll(Tank& tank);
	void draw(svbmath::Vec3& pos, float angle, float yaw);
	void spawn(Tank& tank);
	void update(float dt, std::vector<SmokeEffect*>& smokes, Tank& tank, Sound& sound);
	void strike();
};
