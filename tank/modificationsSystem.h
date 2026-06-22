#pragma once
#include <map>
#include <string>
#include <vector>
#include "tank.h"

extern std::map<std::string, float> modCoefs;
extern std::map<std::string, std::vector<std::string>> modToKeys;


void applyModification(Tank& tank,std::string key);
