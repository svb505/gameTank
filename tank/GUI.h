#pragma once
#include <string>
#include "tank.h"
#include "artillery.h"
#include "sounds.h"
class GUI {
private:
	std::string selectedShell = "";
	std::string controlString = "\nLeft alt - Show/Hide cursor\nSPACE - Shot\nENTER - Machine gun\nW - Move forward\nA - Move left\n"
		"S - Move right\nD - Move back\nShift - Aim mode\n< - Turret to left\n> - Turret ot right\n"
		"^ - Gun to up\nv - Gun to down\n1/2/3 - Change shell\n(APFSDS,HE,SMOKE)\nctrl + ^ - Increase minimap\n"
		"ctrl + v - Decrease minimap";
	float artX = 0.0f;
	float artZ = 0.0f;
public:
	void render(float& fps,Tank& tank,Artillery& art,Sound& sound,bool& fpslimit) {
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
