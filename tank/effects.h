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
        float radius = 2.0f, float height = 1.8f)
        : centerX(x), centerY(y), centerZ(z), duration(durationSec), elapsedTime(0), finished(false),
        radiusScale(radius), heightScale(height)
    {
        particles.reserve(count);
        for (int i = 0; i < count; i++) {

            float u = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
            float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;

            float r = ((float)rand() / RAND_MAX) * radiusScale;

            float x = r * sqrt(1 - u * u) * cos(theta);
            float y = r * u;
            float z = r * sqrt(1 - u * u) * sin(theta);

            float speed = ((float)rand() / RAND_MAX) * 5.0f + 2.0f;

            float vx = 0, vy = 0, vz = 0;
            if (r > 0.0001f) {
                vx = x / r * speed;
                vy = y / r * speed;
                vz = z / r * speed;
            }
            else {
                float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;
                float phi = ((float)rand() / RAND_MAX) * 3.1415926f;
                vx = sin(phi) * cos(theta) * speed;
                vy = cos(phi) * speed;
                vz = sin(phi) * sin(theta) * speed;
            }

            particles.push_back({ x, y, z, vx, vy, vz, durationSec });

        }
    }

    void Update(float dt) {
        if (finished) return;

        elapsedTime += dt;
        if (elapsedTime >= duration) finished = true;

        for (auto& p : particles) {
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.z += p.vz * dt;
            p.life -= dt;
        }
    }

    void Draw() {
        if (finished) return;

        glPushMatrix();
        glTranslatef(centerX, centerY, centerZ);

        glPointSize(2.0f);

        glBegin(GL_POINTS);
        for (auto& p : particles) {
            if (p.life > 0.0f) {
                float intensity = p.life / duration;
                if (intensity > 1.0f) intensity = 1.0f;
                glColor4f(1.0f, 0.5f, 0.0f, intensity);
                glVertex3f(p.x, p.y, p.z);
            }
        }
        glEnd();

        glPopMatrix();
    }

    bool IsFinished() const { return finished; }
    void SetRadius(float r) { radiusScale = r; }
    void SetHeight(float h) { heightScale = h; }
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
        const std::vector<float>& _colors = { 0.2f, 0.2f, 0.2f, 0.5f }, float _size = 1.0f,float _speed = 0.5f,
        float hRadius = 0.5f)
        : centerX(x), centerY(y), centerZ(z), radius(r), maxParticles(count), colors(_colors), size(_size),
        speed(_speed),heightRadius(hRadius)
    {

        particles.reserve(maxParticles);
        for (int i = 0; i < maxParticles; i++) {
            float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;
            float dist = ((float)rand() / RAND_MAX) * radius;
            float height = ((float)rand() / RAND_MAX) * heightRadius;

            particles.push_back({dist * cos(angle),height, dist * sin(angle),0.05f + ((float)rand() / RAND_MAX) * 0.05f,
                0.2f + ((float)rand() / RAND_MAX) * speed});
        }
    }

    std::vector<float> getCoordinates() { return { centerX,centerY,centerZ }; }
    void Update(float dt) {
        for (auto& p : particles) {
            p.y += p.riseSpeed * dt;
            p.x += ((float)rand() / RAND_MAX - 0.5f) * 0.01f;
            p.z += ((float)rand() / RAND_MAX - 0.5f) * 0.01f;
        }
    }
    void Draw() {
        glPushMatrix();
        glTranslatef(centerX, centerY, centerZ);
        glColor4f(colors[0],colors[1],colors[2],colors[3]);
        glPointSize(size);
        glBegin(GL_POINTS);
        for (auto& p : particles) {
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();

        glPopMatrix();
    }
};



