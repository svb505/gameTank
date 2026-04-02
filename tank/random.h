#pragma once
#include <random>

std::mt19937& globalGen() { static std::mt19937 gen(std::random_device{}());
    return gen;
}

float generateFloat() {
    static std::uniform_real_distribution<float> dist(0.5f, 1.0f);
    return dist(globalGen());
}
