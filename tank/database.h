#pragma once
#include <sqlite3.h>
#include <vector>

struct PlayerInfo {
	int score;
	int deaths;
};
struct PlayerContext {
	std::vector<PlayerInfo> players;
};
extern sqlite3* db;

int callback(void* data, int argc, char** argv, char** colName);
void createDb();
bool dbIsExists();
void saveData(int score, int death);
PlayerContext getData();