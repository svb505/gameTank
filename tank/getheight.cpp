#include "getheight.h"
#include <cmath>
#include "craters.h"

float getHeight(float x, float z) {
    float height = 0.0f;

    for (auto& c : craters) {
        float dx = x - c.x;
        float dz = z - c.z;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist < c.radius) {
            float k = exp(-(dist * dist) / (c.radius * c.radius));
            height -= k * c.depth;
        }
    }


    return height;
}