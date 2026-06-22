#include "database.h"
#include <sqlite3.h>
#include <filesystem>
#include "Logger.h"
#include <vector>
#include <string>

sqlite3* db;

int callbackPlayer(void* data, int argc, char** argv, char** colName) {
	PlayerContext* ctx = static_cast<PlayerContext*>(data);
	PlayerInfo p;

	p.kills = std::stoi(argv[1]);
	p.deaths = std::stoi(argv[2]);
	p.score = std::stoi(argv[3]);

	ctx->players.push_back(p);
}
bool dbIsExists(const std::string name) {
	return std::filesystem::exists(name);
}
void createDb(const TypeDb type,const std::string name) {
	std::string createQuery;
	std::string insertQuery;
	char* err;

	if (sqlite3_open(name.c_str(), &db) != SQLITE_OK){
		LOG_ERROR(sqlite3_errmsg(db));
		return;
	}

	if (type == TypeDb::Player) {
		createQuery = "CREATE TABLE if NOT EXISTS player(id INTEGER PRIMARY KEY,"
			"kills INTEGER,death INTEGER,score INTEGER)";
		insertQuery = "INSERT INTO player VALUES(0, 0, 0, 0)";
	}

	if (sqlite3_exec(db, createQuery.c_str(), nullptr, nullptr, &err) != SQLITE_OK){
		LOG_ERROR(err);
		sqlite3_free(err);
		return;
	}
	if (sqlite3_exec(db, insertQuery.c_str(), 0, 0, &err) != SQLITE_OK) {
		LOG_ERROR(err);
		sqlite3_free(err);
		return;
	}
	sqlite3_close(db);
}
void saveDataForPlayer(int kills,int death,int score,std::string scoreOperation) {
	if (sqlite3_open("playerInfo.db", &db) != SQLITE_OK) {
		LOG_ERROR(sqlite3_errmsg(db));
		return;
	}
	char* sql = nullptr;

	if (scoreOperation == "+") {
		sql = sqlite3_mprintf(
			"UPDATE player SET kills = kills + %d, death = death + %d, score = score + %d WHERE id = 0;",
			kills, death, score
		);
	}
	else {
		sql = sqlite3_mprintf(
			"UPDATE player SET kills = kills + %d, death = death + %d, score = score - %d WHERE id = 0;",
			kills, death, score
		);
	}

	char* err = nullptr;

	sqlite3_exec(db, sql, nullptr, nullptr, &err);
	sqlite3_free(sql);
}
PlayerContext getDataForPlayer() {
	PlayerContext ctx;

	if (sqlite3_open("playerInfo.db", &db) != SQLITE_OK) { LOG_ERROR(sqlite3_errmsg(db)); return ctx; }
	else {
		char* err;

		sqlite3_exec(db, "SELECT * FROM player", callbackPlayer, &ctx, &err);
		sqlite3_close(db);

		return ctx;
	}
	
}
