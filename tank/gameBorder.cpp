#include "svbmath.h"
#include "gameBorder.h"
#include "tank.h"
#include "text.h"
#include <format>

svbmath::Vec3 min = {-100,0,-100};
svbmath::Vec3 max = {100,0,100};

float timer = 10.0f;

void checkBorders(Tank& tank,float dt,int ScrH,int ScrW) {
	svbmath::Vec3 pos = tank.getCurrentPos();

	if (pos.x <= min.x || pos.x >= max.x || pos.z <= min.z || pos.z >= max.z) {
		if (timer > 0.0f) {
			timer -= dt;

			std::string str = std::format("Quit the danger zone in {:.0f} s", timer);

			RenderTextHUD(ScrW / 2, ScrH / 2, 1.0f, 0.0f, 0.0f, str.c_str(), ScrW, ScrH);
		}
		else {
			timer = 10.0f;

			tank.getCurrentPos() = tank.getSpawns()[tank.getSelectedSpawn()];
		}

		
	}
}