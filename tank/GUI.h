#pragma once
#include <string>
#include "tank.h"
#include "artillery.h"
#include "sounds.h"
#include "weather.h"

class GUI {
private:
	std::vector<std::string> weathers = { "Clean", "Rainly","Snowly"};
	std::vector<const char*> cstrs;
	int currentIndex = 0;
	std::string selectedShell = "";
	std::string controlString = "\nLeft ctrl - Zoom\nLeft alt - Show/Hide cursor\nSPACE - Shot\nENTER - Machine gun\n"
		"W - Move forward\nA - Move left\nS - Move right\nD - Move back\nShift - Aim mode\n1/2/3 - Change shell\n(APFSDS,HE,SMOKE)\n"
		"ctrl + ^ - Increase minimap\nctrl + v - Decrease minimap";
	float artX = 0.0f;
	float artZ = 0.0f;
public:
	GUI() {
		for (auto& s : weathers) cstrs.push_back(s.c_str());
	}
	void render(float& fps,Tank& tank,Artillery& art,Sound& sound, std::string& weather, bool& fpslimit) {
		std::string buf = std::format("{} / {}", tank.currentHP, tank.HP);
		if (tank.selectedShell == shellType::APFSDS) selectedShell = "APFSDS";
		else if (tank.selectedShell == shellType::SMOKE) selectedShell = "SMOKE";
		else selectedShell = "HE";

		ImGui::Begin("Settings & Info");

		ImGui::InputFloat("X for Artillery strike", &artX);
		ImGui::InputFloat("Z for Artillery strike", &artZ);

		if (ImGui::Button("Start artillery strike")) {
			sound.setSourcePosition(sound.artVolleySource, tank.x, tank.y,tank.z);
			alSourceStop(sound.artVolleySource);
			alSourcePlay(sound.artVolleySource);
			art.spawnShells(artX, artZ);
		}

		ImGui::Text("Strike during: 5s");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("FPS: %.0f", fps);
		ImGui::Checkbox("FPS Limit", &fpslimit);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("My HP: %s",buf.c_str());
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		if (ImGui::Combo("Select weather", &currentIndex, cstrs.data(), cstrs.size())) weather = weathers[currentIndex];
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
