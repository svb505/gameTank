#include "effects.h"

ExplosionEffect::ExplosionEffect(float x, float y, float z, int count, float durationSec,
    float radius, float height) : centerX(x), centerY(y), centerZ(z), duration(durationSec),
    elapsedTime(0), finished(false), radiusScale(radius), heightScale(height){

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

void ExplosionEffect::Update(float dt) {
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
void ExplosionEffect::Draw() {
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
bool ExplosionEffect::IsFinished() const { return finished; }
void ExplosionEffect::SetRadius(float r) { radiusScale = r; }
void ExplosionEffect::SetHeight(float h) { heightScale = h; }
SmokeEffect::SmokeEffect(float x, float y, float z, int count, float r,const std::vector<float>& _colors, 
    float _size, float _speed, float hRadius) : centerX(x), centerY(y), centerZ(z), radius(r), maxParticles(count),
    colors(_colors), size(_size), speed(_speed), heightRadius(hRadius){

    particles.reserve(maxParticles);
    for (int i = 0; i < maxParticles; i++) {
        float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;
        float dist = ((float)rand() / RAND_MAX) * radius;
        float height = ((float)rand() / RAND_MAX) * heightRadius;

        particles.push_back({ dist * cos(angle),height, dist * sin(angle),0.05f + ((float)rand() / RAND_MAX) * 0.05f,
            0.2f + ((float)rand() / RAND_MAX) * speed });
    }
}

std::vector<SmokeEffect::Particle> SmokeEffect::getCoordinates() const {
    std::vector<Particle> coords;
    coords.reserve(particles.size());

    for (auto& p : particles) coords.push_back({ p.x, p.y, p.z });

    return coords;
}
void SmokeEffect::Update(float dt) {
    for (auto& p : particles) {
        p.y += p.riseSpeed * dt;
        p.x += ((float)rand() / RAND_MAX - 0.5f) * 0.01f;
        p.z += ((float)rand() / RAND_MAX - 0.5f) * 0.01f;
    }
}
void SmokeEffect::Draw() {
    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glColor4f(colors[0], colors[1], colors[2], colors[3]);
    glPointSize(size);
    glBegin(GL_POINTS);

    for (auto& p : particles) glVertex3f(p.x, p.y, p.z);

    glEnd();

    glPopMatrix();
}
void updateExplosions(std::vector<ExplosionEffect*>& explosions, float dt) {
    for (auto it = explosions.begin(); it != explosions.end();) {
        (*it)->Update(dt);
        (*it)->Draw();

        if ((*it)->IsFinished()) {
            delete* it;
            it = explosions.erase(it);
        }
        else ++it;
    }
}
void updateSmokes(std::vector<SmokeEffect*>& smokes, float dt) {
    for (auto it = smokes.begin(); it != smokes.end(); ) {
        SmokeEffect* smoke = *it;

        bool alive = false;
        for (auto& p : smoke->getCoordinates()) {
            if (p.y <= 5.0f) {
                alive = true;
                break;
            }
        }
        if (!alive) {
            delete smoke;
            it = smokes.erase(it);
        }
        else {
            smoke->Update(dt);
            smoke->Draw();
            ++it;
        }

    }
}