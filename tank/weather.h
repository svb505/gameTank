#pragma once
#include <vector>
#include <string>
#include "svbmath.h"

class Camera;
class Sound;

enum class Type{
	clear,
	snowly,
	rainly
};

struct Particle {
	Type type;
	float height = 55.0f;
	float x, y, z;
	float vx, vy, vz;
	float gravity = 9.8f;
	float fallSpeed;
};
struct SnowPiles {
	svbmath::Vec3 pos;
	float radius;
	float height;
};

class Weather {
private:
	const int diapazone = 70;
public:
	int count = 130;
	Weather() { particles.reserve(count); }
	
	std::vector<Particle> particles = {};
	std::vector<SnowPiles> snowPiles;

	void drawSnowPile(float radius, float height);
	void drawSphere(float radius, int stacks, int slices);
	void draw();
	void generate(const Type& type,Camera& cam);
	void update(Camera& cam, float dt);
	void generateSnowPiles(int count, float areaSize);
	void renderSnowPiles();
};