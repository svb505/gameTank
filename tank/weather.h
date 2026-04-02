#pragma once
#include <vector>
#include <string>

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

class Weather {
private:
	const int diapazone = 70;
public:
	int count = 130;
	Weather() { particles.reserve(count); }
	
	std::vector<Particle> particles = {};
	void draw();
	void generate(const Type& type,Camera& cam);
	void update(Camera& cam, float dt);
};