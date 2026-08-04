#pragma once
#include <vector>
#include <string>
#include "svbmath.h"
#include <map>

class Camera; class Sound;

enum class Type{
	clean,
	snowy,
	rainy
};

extern std::map<const char*, Type> weathersToConvert;
extern std::vector<const char*> weathersStrings;

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

Type convertStringToType(const char* str);

class Weather {
private:
	const int diapazone = 70;
	bool isCleared = false;

	Type weather = Type::clean;
	int count = 130;

	std::vector<Particle> particles = {};
	std::vector<SnowPiles> snowPiles;
public:
	Weather() { particles.reserve(count); }

	Type& getWeather();

	void drawSnowPile(float radius, float height);
	void drawSphere(float radius, int stacks, int slices);
	void draw();
	void generate(const Type& type,Camera& cam);
	void update(Camera& cam, double dt);
	void generateSnowPiles(int count, float areaSize);
	void renderSnowPiles();
	void getWeather(Sound& sound, Camera& cam);
	void changeTypeOfParticle(const Type& oldType, const Type& newType);
};