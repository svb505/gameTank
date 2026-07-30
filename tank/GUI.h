#pragma once
#include <string>
#include "tank.h"
#include "artillery.h"
#include "sounds.h"
#include "weather.h"
#include "smokeGranade.h"
#include "profiler.h"
#include "enemyes.h"
#include "database.h"
#include "variables.h"
#include "texture.h"
#include "modificationsSystem.h"
#include <format>
#include "input.h"

class GUI {
private:
    Profiler p;

	std::vector<const char*> spawns = { "1", "2","3" };
    std::vector<const char*> days = { "Day","Night"};

	int idxWeather = 0;
	int idxSpawn = 0;
    int idxDay = 0;

	std::string selectedShell = "";
	std::string controlString = "\nLeft ctrl - Zoom\nLeft alt - Show/Hide cursor\nSPACE - Shot\nENTER - Machine gun\n"
		"W - Move forward\nA - Move left\nS - Move right\nD - Move back\nShift - Aim mode\n1/2/3/4 - Change shell\n(APFSDS,HE,ATGM,SMOKE)\n"
		"ctrl + ^ - Increase minimap\nctrl + v - Decrease minimap\nG - Smoke Granade\nR - Range Finder";
	
    float artX = 0.0f;
	float artZ = 0.0f;

	bool artWindow = false;
	bool mlrsWindow = false;
    bool devWindow = false;
    bool statWindow = false;
    bool modWindow = false;
    bool controlWindow = false;
public:
    void setup(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        ImGui::StyleColorsDark();
    }
    void destroy() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void renderBindsWindow() {
        bool banned = false;

        ImGui::Begin("Control Settings");

        for (int i = 0; i < binds.size(); i++){
            for (auto& b : bannedKeysForChanging) {
                if (binds[i].action == b) { banned = true; break; }
            }

            if (banned) { banned = false; continue; }

            ImGui::PushID(i);

            ImGui::Text("%s", binds[i].action.c_str());

            ImGui::SameLine(150);

            if (waitingForBind == i) 
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Click any key...");
            else ImGui::Text("%s", getKeyName(binds[i].key));

            ImGui::SameLine(350);

            if (ImGui::Button("Change")) waitingForBind = i;

            ImGui::PopID();
        }

        ImGui::End();
    }
    void renderMainWin(float& fps,Type& weather, Tank& tank, SmokeGranade& g,bool& locked) {
        std::string buf = std::format("{} / {}", tank.getCurretHp(), tank.getHp());
        selectedShell = shellTypes[tank.getSelectedShell()];

        bool canUseMlrs = (tank.getKills() > 0 && tank.getKills() % 5 == 0);

        ImGui::Begin("Settings & Info");

        if (ImGui::Button("Developper window")) devWindow = true;
        ImGui::SameLine();
        if (ImGui::Button("Show statistick")) statWindow = true;

        if (ImGui::Button("Modifications")) modWindow = true;
        ImGui::SameLine();
        if (ImGui::Button("Control settings")) controlWindow = true;

        if (ImGui::ImageButton("artillery", (ImTextureID)(intptr_t)allTextures["ARTILLERY"], 
            ImVec2(60, 60))) artWindow = true;

        ImGui::SameLine();

        ImGui::BeginDisabled(!canUseMlrs);
        if (ImGui::ImageButton("mlrs", (ImTextureID)(intptr_t)allTextures["MLRS"], ImVec2(60, 60))) mlrsWindow = true;
        ImGui::EndDisabled();

        if (!canUseMlrs && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("You must have a multiple of 5 kills.");
        }

        ImGui::Separator();

        ImGui::Checkbox("Badges in minimap", &badges);
        ImGui::SameLine();
        ImGui::Checkbox("Lock turret", &locked);
        ImGui::Checkbox("FPS Limit", &fpsLimit);
        ImGui::SameLine();
        ImGui::Checkbox("Show Health Bars of enemyes", &showBars);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("FPS: %.0f", fps);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("My HP: %s", buf.c_str());
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("%d / %d", (int)g.getGranades().size(), g.getMaxCount());
        ImGui::Image((ImTextureID)(intptr_t)allTextures["SMOKEGR"], ImVec2(60, 60));

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::Combo("Select weather", &idxWeather, weathersStrings.data(), weathersStrings.size()))
            weather = convertStringToType(weathersStrings[idxWeather]);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::Combo("Select spawn", &idxSpawn, spawns.data(), spawns.size()))
            tank.getSelectedSpawn() = std::stoi(spawns[idxSpawn]);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::Combo("Select day type", &idxDay, days.data(), days.size())) {
            if (idxDay == 0) dayTime = DayTime::DAY;
            else if (idxDay == 1) dayTime = DayTime::NIGHT;

            initLighting();
        }

        ImGui::Separator();

        ImGui::Text("Speed: %.1f", tank.getMoveSpeed());
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("Total shells: %d", tank.getTotalShells());
        ImGui::Image((ImTextureID)(intptr_t)allTextures[selectedShell], ImVec2(70, 60));

        ImGui::Text("Reload time: %.1f", tank.getFinishReload());
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("Kills: %d", tank.getKills());
        ImGui::Text("Score: %d", tank.getScore());
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("Control:");

        for (auto& b : binds) {
            std::string name = getKeyName(b.key);

            ImGui::Text("%s - %s", b.action.c_str(), name.c_str());
        }

        ImGui::End();
    }
    void renderArtilleryWin(Artillery& art,Sound& sound, Tank& tank) {
        ImGui::Begin("Artillery", &artWindow);

        ImGui::InputFloat("X for Artillery strike", &artX);
        ImGui::InputFloat("Z for Artillery strike", &artZ);

        if (ImGui::Button("Start artillery strike")) {
            art.init(8, 25.0f);

            sound.setSourcePosition(sound.sources["ArtVolley"], tank.getCurrentPos());
            alSourceStop(sound.sources["ArtVolley"]);
            alSourcePlay(sound.sources["ArtVolley"]);

            art.spawnShells(artX, artZ);
        }

        ImGui::End();
    }
    void renderMlrsWin(Artillery& art,Sound& sound,Tank& tank) {
        ImGui::Begin("MLRS", &mlrsWindow);

        ImGui::InputFloat("X for MLRS strike", &artX);
        ImGui::InputFloat("Z for MLRS strike", &artZ);

        if (ImGui::Button("Start MLRS strike")) {
            art.init(25, 125.0f);

            sound.setSourcePosition(sound.sources["ArtVolley"], tank.getCurrentPos());
            alSourceStop(sound.sources["ArtVolley"]);
            alSourcePlay(sound.sources["ArtVolley"]);

            art.spawnShells(artX, artZ);
        }

        ImGui::Text("Strike duration: 5s");

        ImGui::End();
    }
    void renderDevWin(Sound& sound,std::unordered_map<int, Entity>& enemyes) {
        ImGui::Begin("Dev. Window", &devWindow);

        ImGui::Text("%s\n%s", p.getMemoryUsage()[0].c_str(), p.getMemoryUsage()[1].c_str());
        ImGui::Text("Enemyes count: %d", enemyes.size());
        ImGui::Text("Sound buffers: %d", sound.buffers.size());
        ImGui::Text("Sound sources: %d", sound.sources.size());

        ImGui::End();
    }
    void renderStatWin() {
        if (!dbIsExists("playerInfo.db")) ImGui::Text("DB is not exists");
        else {
            ImGui::Begin("Statistick", &statWindow);

            PlayerContext ctx;

            ctx = getDataForPlayer();

            for (const auto& c : ctx.players) {
                float kd = (c.deaths == 0) ? (float)c.kills : (float)c.kills / (float)c.deaths;

                ImGui::Text("Total kills: %d", c.kills);
                ImGui::Text("Total deaths: %d", c.deaths);
                ImGui::Text("Total score: %d", c.score);
                ImGui::Text("KD: %.1f", kd);
            }

            ImGui::End();
        }
    }
    void renderModificationWin(Tank& tank) {
        ImGui::Begin("Modifications");

        PlayerContext ctx;

        ctx = getDataForPlayer();

        for (const auto& c : ctx.players) {
            std::map<std::string, Modification> modf = getModifications("modifications");
            
            std::string score = std::format("Total score: {}", c.score);
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.5f, 1.0f), score.c_str());
            ImGui::Dummy({ 0.0f, 10.0f });

            for (auto& m : modf) {
                std::string name = std::format("Name: {}", m.first);

                ImGui::TextColored(ImVec4(0.0f,0.5f,1.0f,1.0f),name.c_str());
                
                if (m.second.active) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Modification buyed");
                    ImGui::Dummy({ 0.0f,10.0f });
                }
                else {
                    std::string buyText = std::format("Buy {}",m.first);
                    
                    ImGui::Text("Price: %d pts", m.second.value);
                    
                    if (ImGui::Button(buyText.c_str())) applyModification(tank,m.first);
                    
                    ImGui::Dummy({ 0.0f,10.0f });
                }         
            }         
        }

        ImGui::End();
    }
    
    void render(float& fps, Tank& tank, Artillery& art, Sound& sound,Type& weather, 
        SmokeGranade& g,bool& badges,std::unordered_map<int, Entity>& enemyes,
        bool& locked) {
        
        renderMainWin(fps,weather,tank,g,locked);

        if (modWindow) renderModificationWin(tank);
        if (statWindow) renderStatWin();
        if (devWindow) renderDevWin(sound, enemyes);
        if (artWindow) renderArtilleryWin(art,sound,tank);
        if (mlrsWindow) renderMlrsWin(art,sound,tank);
        if (controlWindow) renderBindsWindow();
    }
};
