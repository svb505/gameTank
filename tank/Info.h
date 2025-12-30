#pragma once
#include <vector>
#include <string>
#include "enemyes.h" 

class Enemy;
struct Info {
    Enemy* enemy;
    std::vector<float> positions;
    std::string type;
};
