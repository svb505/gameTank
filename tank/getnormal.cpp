#include "getnormal.h"
#include "getheight.h"
#include "svbmath.h"

svbmath::Vec3 getNormal(float x, float z) {
    float delta = 0.5f;

    float hL = getHeight(x - delta, z);
    float hR = getHeight(x + delta, z);
    float hD = getHeight(x, z - delta);
    float hU = getHeight(x, z + delta);

    svbmath::Vec3 normal = svbmath::Normalize(svbmath::Vec3(hL - hR, 2.0f, hD - hU));

    return normal;
}