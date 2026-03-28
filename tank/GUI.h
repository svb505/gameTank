#pragma once
#include <string>
#include "tank.h"
#include "artillery.h"

class GUI {
private:
	std::string selectedShell = "";
	std::string controlString = "\nSPACE - Shot\nENTER - Machine gun\nW - Move forward\nA - Move left\n"
		"S - Move right\nD - Move back\nQ - Camera to left\nE - Camera to right \nShift - Aim mode\n< - Turret to left\n"
		"> - Turret ot right\n^ - Gun to up\nv - Gun to down\n1/2/3 - Change shell\n(APFSDS,HE,SMOKE)\n"
		"ctrl + ^ - Increase minimap\nctrl + v - Decrease minimap";
	float artX = 0.0f;
	float artZ = 0.0f;
public:
	void render(float& fps,Tank& tank,Artillery& art) {
		if (tank.selectedShell == shellType::APFSDS) selectedShell = "APFSDS";
		else if (tank.selectedShell == shellType::SMOKE) selectedShell = "SMOKE";
		else selectedShell = "HE";

		ImGui::Begin("Settings & Info");

		ImGui::InputFloat("X for Artillery strike", &artX);
		ImGui::InputFloat("Z for Artillery strike", &artZ);

		if (ImGui::Button("Start artillery strike")){
			art.spawnShells(artX,artZ);
		}

		ImGui::Text("FPS: %.0f", fps);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

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
