#pragma once
#include <iostream>
#include <vector>
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <chrono>
#include <cstdlib>

class ExplosionEffect {
private:
    struct Particle {
        float x, y, z;
        float vx, vy, vz;
        float life;
    };

    std::vector<Particle> particles;
    float duration;
    float elapsedTime;
    bool finished;
    float centerX, centerY, centerZ;

    float radiusScale;
    float heightScale;

public:
    ExplosionEffect(float x, float y, float z, int count = 400, float durationSec = 6.0f,
        float radius = 2.0f, float height = 1.8f);

    void Update(float dt);
    void Draw();
    bool IsFinished() const;
    void SetRadius(float r);
    void SetHeight(float h);
};
class SmokeEffect {
private:
    struct Particle {
        float x, y, z;
        float size;
        float riseSpeed;
    };

    std::vector<Particle> particles;
    int maxParticles;
    float centerX, centerY, centerZ;
    float radius;
    float size;
    float speed;
    float heightRadius;
    std::vector<float> colors = { 0.2f, 0.2f, 0.2f, 0.2f };

public:
    SmokeEffect(float x, float y, float z, int count = 100, float r = 1.0f,
        const std::vector<float>& _colors = { 0.2f, 0.2f, 0.2f, 0.5f }, float _size = 1.0f, float _speed = 0.5f,
        float hRadius = 0.5f);
    std::vector<Particle> getCoordinates() const;
    void Update(float dt);
    void Draw();
};
void updateExplosions(std::vector<ExplosionEffect*>& explosions, float dt);
void updateSmokes(std::vector<SmokeEffect*>& smokes, float dt);