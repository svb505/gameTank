#pragma once
#include <vector>

struct artShell {
	float x, y, z;
};
class Artillery {
private:
	int volley = 10;
	float shellHeight = 40.0f;
	float shellSpeed = 25.0f;
public:
	std::vector<artShell> shells;

	std::vector<float> returnRandomError();
	void drawShell();
	void spawnShells(float x,float z);
	void updateShells(float dt);
};