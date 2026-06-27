#pragma once
#include <vector>
#include "projectile.h"

class Artillery {
private:
	int volley = 10;
	float shellHeight = 40.0f;
	float shellSpeed = 25.0f;
	float waiting = 5.0f;

	std::vector<Projectile> shells;
public:
	Artillery();

	std::vector<Projectile>& getShells();
	std::vector<float> returnRandomError();

	void init(int volley,float shellSpeed);
	void spawnShells(float x,float z);
	void updateShells(float dt);
	void drawAllShells();
	void deleteIfAlived();
	float returnSpeedError();
};