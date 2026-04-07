#pragma once
#include <string>
#include "tank.h"
#include "artillery.h"
#include "sounds.h"
#include "weather.h"
#include "smokeGranade.h"

class GUI {
private:
	std::vector<std::string> weathers = { "Clean", "Rainly","Snowly"};
	std::vector<std::string> spawns = { "1", "2","3" };
	std::vector<const char*> cstrsW;
	std::vector<const char*> cstrsS;
	int idxW = 0;
	int idxS = 0;
	std::string selectedShell = "";
	std::string controlString = "\nLeft ctrl - Zoom\nLeft alt - Show/Hide cursor\nSPACE - Shot\nENTER - Machine gun\n"
		"W - Move forward\nA - Move left\nS - Move right\nD - Move back\nShift - Aim mode\n1/2/3/4 - Change shell\n(APFSDS,HE,ATGM,SMOKE)\n"
		"ctrl + ^ - Increase minimap\nctrl + v - Decrease minimap\nG - Smoke Granade";
	float artX = 0.0f;
	float artZ = 0.0f;
	bool artWindow = false;
public:
	GUI() {
		for (auto& s : weathers) cstrsW.push_back(s.c_str());
		for (auto& s : spawns) cstrsS.push_back(s.c_str());
	}
	void render(float& fps,Tank& tank,Artillery& art,Sound& sound, std::string& weather, SmokeGranade& g,bool& badges,
		bool& fpslimit) {
		std::string buf = std::format("{} / {}", tank.currentHP, tank.HP);
		if (tank.selectedShell == shellType::APFSDS) selectedShell = "APFSDS";
		else if (tank.selectedShell == shellType::SMOKE) selectedShell = "SMOKE";
		else if (tank.selectedShell == shellType::ATGM) selectedShell = "ATGM";
		else selectedShell = "HE";

		ImGui::Begin("Settings & Info");

		if (ImGui::Button("Aimer artillery strike")) artWindow = true;

		if (artWindow) {
			ImGui::Begin("Artillery");
			ImGui::InputFloat("X for Artillery strike", &artX);
			ImGui::InputFloat("Z for Artillery strike", &artZ);

			if (ImGui::Button("Start artillery strike")) {
				sound.setSourcePosition(sound.artVolleySource, tank.x, tank.y, tank.z);
				alSourceStop(sound.artVolleySource);
				alSourcePlay(sound.artVolleySource);
				art.spawnShells(artX, artZ);
			}
			if (ImGui::Button("Close")) artWindow = false;
			ImGui::End();
		}

		

		ImGui::Text("Strike during: 5s");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Checkbox("Badges in minimap",&badges);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("FPS: %.0f", fps);
		ImGui::Checkbox("FPS Limit", &fpslimit);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("My HP: %s",buf.c_str());
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Smoke granades %d / %d", g.granades.size(),g.maxCount);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		if (ImGui::Combo("Select weather", &idxW, cstrsW.data(), cstrsW.size())) weather = weathers[idxW];
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		if (ImGui::Combo("Select spawn", &idxS, cstrsS.data(), cstrsS.size())) tank.selectedSpawn = std::stoi(spawns[idxS]);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Speed: %.1f", tank.moveSpeed);
		ImGui::Text("Total shells: %d", tank.totalShells);
		ImGui::Text("Selected shell: %s", selectedShell);
		ImGui::Text("Reload time: %.1f", tank.finishReload);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("Kills: %d", tank.kills);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("Control: %s", controlString.c_str());

		ImGui::End();
	}
};
