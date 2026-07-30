#pragma once
#include <string>
#include <map>

struct Modification {
    int value;
    bool active;
};
std::map<std::string, Modification> getModifications(const std::string& key);

void changeKeyBindConfig(const std::string& key,const int value);
void changeModificationConfig(const std::string& key);
void changePlayerConfig(const std::string key, const float value);
float getFloatFromJson(std::string key,std::string path = "gameConfig.json");


