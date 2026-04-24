#include "json_parser.h"
#include <string>
#include <fstream>
#include <nlohmann//json.hpp>

using json = nlohmann::json;

float getFloatFromJson(std::string key) {
	std::ifstream file("gameConfig.json");

	json j;

	file >> j;

	if (j.contains(key)) return j[key];
	else return 1.0f;
}