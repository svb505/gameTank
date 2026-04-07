#pragma once
#include <vector>
#include "projectile.h"

class Artillery {
private:
	int volley = 10;
	float shellHeight = 40.0f;
	float shellSpeed = 25.0f;
	float waiting = 5.0f;
public:
	std::vector<Projectile> shells;

	std::vector<float> returnRandomError();
	void spawnShells(float x,float z);
	void updateShells(float dt);
	void drawAllShells();
	void deleteIfAlived();
	float returnSpeedError();
};