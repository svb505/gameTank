#include "APSSystem.h"
#include "sounds.h"
#include "projectileSystem.h"
#include "svbmath.h"
#include "effects.h"
#include "tank.h"

float MAX_SHL_SPD = 140.0f;
float RADIUS = 5.0f;
int counter_munition = 10;

bool shellInRadius(const svbmath::Vec3& shellPos, const svbmath::Vec3 tankPos, float radius) {
    svbmath::Vec3 diff = tankPos - shellPos;
    return LengthSq(diff) <= radius * radius;
}
void startAPS(Projectile& p, EffectsContext& context, Sound& sound,Tank& tank) {
    if (shellInRadius(p.pos, tank.getCurrentPos(), RADIUS) && p.speed <= 
        MAX_SHL_SPD && p.isEnemy && counter_munition > 10) {
        counter_munition--;
        
        context.explosions.push_back(new ExplosionEffect(p.pos, 300, 3.0f, 1.5f, 1.0f));
        context.smokes.push_back(new SmokeEffect(p.pos, 80.0f, 1.0));

        sound.setSourcePosition(sound.sources["Explosion"], p.pos);
        alSourceStop(sound.sources["Explosion"]);
        alSourcePlay(sound.sources["Explosion"]);

        p.alive = false;
    }
}
