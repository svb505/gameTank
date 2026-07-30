#include "json_parser.h"
#include <string>
#include <fstream>
#include <nlohmann//json.hpp>

using json = nlohmann::json;

void changeKeyBindConfig(const std::string& key, const int value) {
	std::ifstream file("keyBinds.json");
	json j;

	file >> j;

	j[key] = value;

	std::ofstream("keyBinds.json") << j.dump(4);
}
void changePlayerConfig(const std::string key, const float value) {
	std::ifstream file("gameConfig.json");
	json j;

	file >> j;

	j[key] = value;

	std::ofstream("gameConfig.json") << j.dump(4);
}
void changeModificationConfig(const std::string& key) {
	std::ifstream file("modificationsConfig.json");
	json j;

	file >> j;

	j["modifications"][key][1] = true;

	std::ofstream("modificationsConfig.json") << j.dump(4);
}
void from_json(const json& j, Modification& m) {
	m.value = j.at(0).get<int>();
	m.active = j.at(1).get<bool>();
}
std::map<std::string, Modification> getModifications(const std::string& key) {
	std::ifstream file("modificationsConfig.json");
	json j;
	file >> j;

	return j.at(key).get<std::map<std::string, Modification>>();
}
float getFloatFromJson(std::string key,std::string path) {
	std::ifstream file(path);

	json j;

	file >> j;

	if (j.contains(key)) return j[key];
	else return 1.0f;
}