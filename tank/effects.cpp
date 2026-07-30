#include "effects.h"
#include "svbmath.h"
#include "texture.h"

ExplosionEffect::ExplosionEffect(svbmath::Vec3 pos, int count, float durationSec,
    float radius, float height) : centers(pos.x,pos.y,pos.z), duration(durationSec),
    elapsedTime(0), finished(false), radiusScale(radius), heightScale(height){

    particles.reserve(count);

    //Generate particles
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

        particles.push_back({ {x, y, z}, {vx, vy, vz}, durationSec });

    }
}

void ExplosionEffect::Update(float dt) {
    if (finished) return;

    elapsedTime += dt;
    if (elapsedTime >= duration) finished = true;

    for (auto& p : particles) {
        p.pos.x += p.velovity.x * dt;
        p.pos.y += p.velovity.y * dt;
        p.pos.z += p.velovity.z * dt;
        p.life -= dt;
    }
}
void ExplosionEffect::Draw(){
    GLuint explosionTex = allTextures["fire"];

    if (finished) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, explosionTex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPushMatrix();
    glTranslatef(centers.x, centers.y, centers.z);

    glBegin(GL_QUADS);

    for (auto& p : particles){
        if (p.life > 0.0f){
            float i = p.life / duration;
            if (i > 1.0f) i = 1.0f;

            float s = 0.2f;

            glColor4f(1, 1, 1, i);

            glTexCoord2f(0, 0); glVertex3f(p.pos.x - s, p.pos.y - s, p.pos.z);
            glTexCoord2f(1, 0); glVertex3f(p.pos.x + s, p.pos.y - s, p.pos.z);
            glTexCoord2f(1, 1); glVertex3f(p.pos.x + s, p.pos.y + s, p.pos.z);
            glTexCoord2f(0, 1); glVertex3f(p.pos.x - s, p.pos.y + s, p.pos.z);
        }
    }

    glEnd();

    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
bool ExplosionEffect::IsFinished() const { return finished; }
void ExplosionEffect::SetRadius(float r) { radiusScale = r; }
void ExplosionEffect::SetHeight(float h) { heightScale = h; }
SmokeEffect::SmokeEffect(svbmath::Vec3 pos, int count, float r,const std::vector<float>& _colors, 
    float _size, float _speed, float hRadius) : centers(pos.x, pos.y, pos.z), radius(r), maxParticles(count),
    colors(_colors), size(_size), speed(_speed), heightRadius(hRadius){

    particles.reserve(maxParticles);

    for (int i = 0; i < maxParticles; i++) {
        float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;
        float dist = ((float)rand() / RAND_MAX) * radius;
        float height = ((float)rand() / RAND_MAX) * heightRadius;

        particles.push_back({ {dist * cos(angle),height, dist * sin(angle)},
            0.05f + ((float)rand() / RAND_MAX) * 0.05f,
            0.2f + ((float)rand() / RAND_MAX) * speed });
    }
}

std::vector<SmokeEffect::Particle> SmokeEffect::getCoordinates() const {
    std::vector<Particle> coords;
    coords.reserve(particles.size());

    for (auto& p : particles) coords.push_back({ p.pos });

    return coords;
}
void SmokeEffect::Update(float dt) {
    for (auto& p : particles) {
        p.pos.y += p.riseSpeed * dt;
        p.pos.x += ((float)rand() / RAND_MAX - 0.5f) * 0.01f;
        p.pos.z += ((float)rand() / RAND_MAX - 0.5f) * 0.01f;
    }
}
void SmokeEffect::Draw(){
    GLuint smokeTex = allTextures["smoke"];

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, smokeTex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1, 1, 1, 1);

    glPushMatrix();
    glTranslatef(centers.x, centers.y, centers.z);

    glBegin(GL_QUADS);

    for (auto& p : particles){
        float s = size * 0.1f;

        glTexCoord2f(0, 0); glVertex3f(p.pos.x - s, p.pos.y - s, p.pos.z);
        glTexCoord2f(1, 0); glVertex3f(p.pos.x + s, p.pos.y - s, p.pos.z);
        glTexCoord2f(1, 1); glVertex3f(p.pos.x + s, p.pos.y + s, p.pos.z);
        glTexCoord2f(0, 1); glVertex3f(p.pos.x - s, p.pos.y + s, p.pos.z);
    }

    glEnd();

    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
void updateExplosions(std::vector<ExplosionEffect*>& explosions, float dt) {
    for (auto it = explosions.begin(); it != explosions.end();) {
        (*it)->Update(dt);
        (*it)->Draw();

        if ((*it)->IsFinished()) { delete* it; it = explosions.erase(it); }
        else ++it;
    }
}
void updateSmokes(std::vector<SmokeEffect*>& smokes, float dt) {
    for (auto it = smokes.begin(); it != smokes.end(); ) {
        SmokeEffect* smoke = *it;

        bool alive = false;
        for (auto& p : smoke->getCoordinates()) {
            if (p.pos.y <= 5.0f) { alive = true; break; }
        }
        if (!alive) { delete smoke; it = smokes.erase(it); }

        else {
            smoke->Update(dt);
            smoke->Draw();
            ++it;
        }

    }
}