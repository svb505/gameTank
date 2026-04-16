#include "effects.h"
#include "smokeGranade.h"
#include "tank.h"
#include <cmath>
#include "sounds.h"
#include "svbmath.h"

void SmokeGranade::draw(svbmath::Vec3& pos,float angle,float yaw) {
    float s = 0.1f;

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    glRotatef(yaw, 0, 1, 0);
    glRotatef(angle, 1, 0, 0);

    glColor3f(0.3f, 0.3f, 0.3f);

    glBegin(GL_QUADS);
    // front
    glVertex3f(-s, -s, s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(-s, s, s);
    // back
    glVertex3f(-s, -s, -s); glVertex3f(s, -s, -s); glVertex3f(s, s, -s); glVertex3f(-s, s, -s);
    // left
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s); glVertex3f(-s, s, s); glVertex3f(-s, s, -s);
    // right
    glVertex3f(s, -s, -s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(s, s, -s);
    // top
    glVertex3f(-s, s, -s); glVertex3f(-s, s, s); glVertex3f(s, s, s); glVertex3f(s, s, -s);
    // bottom
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s); glVertex3f(s, -s, s); glVertex3f(s, -s, -s);
    glEnd();
    glPopMatrix();
}
void SmokeGranade::drawAll(Tank& tank) {
    for (auto& g : granades) draw(g.pos,g.angle,tank.turretYaw);
}
void SmokeGranade::spawn(Tank& tank) {
    float halfW = tank.params.turretW * 0.5f;

    svbmath::Vec3 leftLocal = { -halfW, 0.0f, 0.0f };
    svbmath::Vec3 rightLocal = { halfW, 0.0f, 0.0f };
    svbmath::Vec3 leftSide = tank.LocalToWorldTurret(leftLocal);
    svbmath::Vec3 rightSide = tank.LocalToWorldTurret(rightLocal);

    for (int i = 0; i < maxCount; i++) {
        Position p;

        if (i < 3) p.pos = leftSide;
        else p.pos = rightSide;

        float angleRad = p.angle * PI / 180.0f;
        float yawRad = (tank.bodyYaw + tank.turretYaw) * PI / 180.0f;

        p.count = 3000;

        p.velocity.x = sin(yawRad) * cos(angleRad) * p.speed; 
        p.velocity.y = sin(p.angle) * p.speed;
        p.velocity.z = cos(yawRad) * cos(angleRad) * p.speed;

        granades.push_back(p);
    }
}
void SmokeGranade::update(float dt, std::vector<SmokeEffect*>& smokes,Tank& tank,Sound& sound) {
    float halfW = tank.params.turretW * 0.5f; 

    svbmath::Vec3 leftLocal = { -halfW, 0.0f, 0.0f }; 
    svbmath::Vec3 rightLocal = { halfW, 0.0f, 0.0f }; 
    svbmath::Vec3 leftSide = tank.LocalToWorldTurret(leftLocal); 
    svbmath::Vec3 rightSide = tank.LocalToWorldTurret(rightLocal);

    int i = 0;

    for (auto& g : granades) {     
        if (!g.strike) {
            float angleRad = g.angle * PI / 180.0f;
            float yawRad = (tank.bodyYaw + tank.turretYaw) * PI / 180.0f;

            g.pos = (i < maxCount / 2) ? tank.LocalToWorldTurret(leftLocal) : tank.LocalToWorldTurret(rightLocal);
            
            g.velocity.x = sin(yawRad) * cos(angleRad) * g.speed;
            g.velocity.y = sin(g.angle) * g.speed;
            g.velocity.z = cos(yawRad) * cos(angleRad) * g.speed;
        }
        else{
            g.velocity.y -= g.gravity * dt;

            g.pos.x += g.velocity.x * dt;
            g.pos.y += g.velocity.y * dt;
            g.pos.z += g.velocity.z * dt;
        }
        i++;
    }

    for (auto it = granades.begin(); it != granades.end();) {
        auto& g = (*it);
        if (g.pos.y <= 0.0f) {
            smokes.push_back(new SmokeEffect(g.pos, g.count, 7.0f, { 1.0f,1.0f,1.0f }, 3.0f,
                0.1f,9.0f));
            sound.setSourcePosition(sound.sources["Smoke"], g.pos);
            alSourcePlay(sound.sources["Smoke"]);

            it = granades.erase(it);
        }
        else ++it;
    }
    i = 0;
}
void SmokeGranade::strike() {
    if (currentGranade < granades.size()) {
        granades[currentGranade].strike = true;
        currentGranade++;
    }
}