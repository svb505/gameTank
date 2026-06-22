#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

enum class TypeDb {
	Player, Modifications
};

struct PlayerInfo {
	int kills;
	int deaths;
	int score;
};
struct PlayerContext {
	std::vector<PlayerInfo> players;
};
extern sqlite3* db;

int callbackPlayer(void* data, int argc, char** argv, char** colName);
void createDb(const TypeDb type,const std::string name = "playerInfo.db");
bool dbIsExists(const std::string name = "playerInfo.db");
void saveDataForPlayer(int kills, int death,int score,std::string scoreOperation);
PlayerContext getDataForPlayer();