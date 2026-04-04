#pragma once
#include "svbmath.h"
#include <vector>

struct Position {
	svbmath::Vec3 pos;
	float speed = 15.0f;
	float vx, vy, vz;
	float gravity = 9.8f;
	int count;
	float duration;
};

class SmokeGranade {
public:
	std::vector<Position> granades;

	void drawAll();
	void draw(float x, float y, float z);
	void spawn(float x, float y, float z);
	void update();

};
