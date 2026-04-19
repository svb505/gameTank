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

class GUI {
private:
    Profiler p;

	std::vector<std::string> weathers = { "Clean", "Rainly","Snowly"};
	std::vector<std::string> spawns = { "1", "2","3" };
	std::vector<const char*> cstrsW;
	std::vector<const char*> cstrsS;

	int idxW = 0;
	int idxS = 0;

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
public:
	GUI() {
		for (auto& s : weathers) cstrsW.push_back(s.c_str());
		for (auto& s : spawns) cstrsS.push_back(s.c_str());
	}
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
    void render(float& fps, Tank& tank, Artillery& art, Sound& sound,std::string& weather, SmokeGranade& g,bool& badges,
        bool& fpslimit, std::unordered_map<int, Entity>& enemyes,bool& locked){
        std::string buf = std::format("{} / {}", tank.currentHP, tank.HP);

        if (tank.selectedShell == shellType::APFSDS) selectedShell = "APFSDS";
        else if (tank.selectedShell == shellType::SMOKE) selectedShell = "SMOKE";
        else if (tank.selectedShell == shellType::ATGM) selectedShell = "ATGM";
        else selectedShell = "HE";

        bool canUseMlrs = (tank.kills > 0 && tank.kills % 5 == 0);

        ImGui::Begin("Settings & Info");

        if (ImGui::Button("Show statistick")) statWindow = true;
        if (ImGui::Button("Aimer artillery strike")) artWindow = true;
        if (ImGui::Button("Developper window")) devWindow = true;

        ImGui::BeginDisabled(!canUseMlrs);
        if (ImGui::Button("Aimer MLRS strike")) mlrsWindow = true;
        ImGui::EndDisabled();

        if (!canUseMlrs && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)){
            ImGui::SetTooltip("You must have a multiple of 5 kills.");
        }

        ImGui::Separator();

        ImGui::Checkbox("Badges in minimap", &badges);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Checkbox("Show Health Bars of enemyes", &showBars);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("FPS: %.0f", fps);
        ImGui::Checkbox("FPS Limit", &fpslimit);

        ImGui::Text("My HP: %s", buf.c_str());

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Checkbox("Lock turret", &locked);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("Smoke grenades %d / %d", (int)g.granades.size(), g.maxCount);
        
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::Combo("Select weather", &idxW, cstrsW.data(), cstrsW.size())) weather = weathers[idxW];
        
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        
        if (ImGui::Combo("Select spawn", &idxS, cstrsS.data(), cstrsS.size())) tank.selectedSpawn = std::stoi(spawns[idxS]);

        ImGui::Separator();

        ImGui::Text("Speed: %.1f", tank.moveSpeed);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("Total shells: %d", tank.totalShells);
        ImGui::Text("Selected shell: %s", selectedShell);
        ImGui::Text("Reload time: %.1f", tank.finishReload);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("Kills: %d", tank.kills);
        ImGui::Text("Score: %d", tank.score);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("Control: %s", controlString.c_str());

        ImGui::End();

        if (statWindow) {
            if (!dbIsExists()) ImGui::Text("DB is not exists");
            else {
                ImGui::Begin("Statistick", &statWindow);

                PlayerContext ctx;

                ctx = getData();

                for (const auto& c : ctx.players) {
                    float kd = (c.deaths == 0) ? (float)c.score : (float)c.score / (float)c.deaths;

                    ImGui::Text("Total kills: %d",c.score);
                    ImGui::Text("Total deaths: %d",c.deaths);
                    ImGui::Text("KD: %.1f",kd);
                }

                ImGui::End();
            }
        }
        if (devWindow) {
            ImGui::Begin("Dev. Window", &devWindow);

            ImGui::Text("%s\n%s", p.getMemoryUsage()[0].c_str(), p.getMemoryUsage()[1].c_str());
            ImGui::Text("Enemyes count: %d",enemyes.size());
            ImGui::Text("Sound buffers: %d", sound.buffers.size());
            ImGui::Text("Sound sources: %d", sound.sources.size());

            ImGui::End();
        }
        if (artWindow){
            ImGui::Begin("Artillery", &artWindow);

            ImGui::InputFloat("X for Artillery strike", &artX);
            ImGui::InputFloat("Z for Artillery strike", &artZ);

            if (ImGui::Button("Start artillery strike")){
                art.init(8, 25.0f);

                sound.setSourcePosition(sound.sources["ArtVolley"], tank.pos);
                alSourceStop(sound.sources["ArtVolley"]);
                alSourcePlay(sound.sources["ArtVolley"]);

                art.spawnShells(artX, artZ);
            }

            ImGui::End();
        }
        if (mlrsWindow){
            ImGui::Begin("MLRS", &mlrsWindow);

            ImGui::InputFloat("X for MLRS strike", &artX);
            ImGui::InputFloat("Z for MLRS strike", &artZ);

            if (ImGui::Button("Start MLRS strike")){
                art.init(25, 125.0f);

                sound.setSourcePosition(sound.sources["ArtVolley"], tank.pos);
                alSourceStop(sound.sources["ArtVolley"]);
                alSourcePlay(sound.sources["ArtVolley"]);

                art.spawnShells(artX, artZ);
            }

            ImGui::Text("Strike duration: 5s");

            ImGui::End();
        }
    }
};
