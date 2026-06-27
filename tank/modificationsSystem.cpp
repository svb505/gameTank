#include "modificationsSystem.h"
#include "json_parser.h"
#include "database.h"
#include <map>
#include <string>
#include "tank.h"

std::map<std::string, float> modCoefs = { {"engine",1.5f},
                                        {"turret_rotation_mechanism",1.2f},
                                        {"gun_elevation_mechanism",1.4f},{"gun",0.9f} };
std::map<std::string, std::vector<std::string>> modToKeys = {
                                        {"engine",{"MAX_FORWARD_SPEED","MAX_BACK_SPEED"}},
                                        {"turret_rotation_mechanism",{"tankTurretSpeed"}},
                                        {"gun_elevation_mechanism",{"tankGunSpeed"}},
                                        {"gun",{"tankReload"}} };

void applyModification(Tank& tank,std::string key) {
	PlayerContext ctx;

	ctx = getDataForPlayer();

    std::map<std::string, Modification> modf = getModifications("modifications");

    for (const auto& c : ctx.players) {
        if (c.score < modf[key].value) return;
        else {
            saveDataForPlayer(tank.getKills(),tank.getDeath(), modf[key].value, "-");

            for (auto k : modToKeys[key]) {
                float newValue = getFloatFromJson(k) * modCoefs[key];

                changePlayerConfig(k,newValue);
            }

            changeModificationConfig(key);
            
        }
    }
}