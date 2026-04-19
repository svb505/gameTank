#include "database.h"
#include <sqlite3.h>
#include <filesystem>
#include "Logger.h"
#include <vector>

sqlite3* db;

int callback(void* data, int argc, char** argv, char** colName) {
	PlayerContext* ctx = static_cast<PlayerContext*>(data);
	PlayerInfo p;

	p.score = std::stoi(argv[1]);
	p.deaths = std::stoi(argv[2]);
	
	ctx->players.push_back(p);
}
bool dbIsExists() {
	return std::filesystem::exists("playerInfo.db");
}
void createDb() {
	sqlite3_open("playerInfo.db", &db);

	const char* sql = "CREATE TABLE if NOT EXISTS player(id INTEGER PRIMARY KEY,score INTEGER,death INTEGER)";
	char* err;

	sqlite3_exec(db,sql,0,0,&err);
	sqlite3_exec(db, "INSERT INTO player VALUES(0, 0, 0)", 0, 0, &err);
}
void saveData(int score,int death) {
	if (sqlite3_open("playerInfo.db", &db) != SQLITE_OK) {
		LOG_ERROR(sqlite3_errmsg(db));
		return;
	}

	char* sql = sqlite3_mprintf("UPDATE player SET score = score + %d, death = death + %d WHERE id = 0;", score,death);
	char* err;

	sqlite3_exec(db,sql,0,0,&err);

	sqlite3_free(sql);
}
PlayerContext getData() {
	PlayerContext ctx;

	if (sqlite3_open("playerInfo.db", &db) != SQLITE_OK) { LOG_ERROR(sqlite3_errmsg(db)); return ctx; }
	else {
		char* err;

		sqlite3_exec(db, "SELECT * FROM player", callback, &ctx, &err);

		return ctx;
	}
	
}
