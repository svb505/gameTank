#pragma once
#include <vector>
#include <string>
#include "svbmath.h"

struct ColoredWord{
	std::string text;
	float r, g, b;
};

struct Message {
	std::vector<ColoredWord> words;
	float life = 3.0f;
};

extern std::vector<Message> messages;

void updateKillChat(float dt);
void addToKillChat(std::string killer, std::string killed, std::string shell, int killerId, int killedId);
void showKillChat(int WW, int WH);