#include <vector>
#include "text.h"
#include "killchat.h"
#include "svbmath.h"
#include <format>

std::vector<Message> messages = {};

void addToKillChat(std::string killer,std::string killed,std::string shell,int killerId, int killedId) {
	std::string s1 = std::format("{}({})",killer,killerId);
	std::string s2 = std::format("{}", shell);
	std::string s3 = std::format("{}({})", killed, killedId);

	Message m;

	m.words.push_back({s1,0,0,1.5f});
	m.words.push_back({ s2,1,1,1 });
	m.words.push_back({ s3,1,0,0 });

	messages.push_back(m);
}
void updateKillChat(float dt) {
	for (auto& m : messages) m.life -= dt;

	for (auto it = messages.begin();it != messages.end();) {
		if ((*it).life <= 0.0f) it = messages.erase(it);
		else ++it;
	}
}
void showKillChat(int WW,int WH) {
	const int miniW = WW / 4;
	const float baseX = WW - miniW;
	float baseY = WH / 2 + 200;
	float step = 5.0f;

	for (auto& m : messages){
		RenderTextHUD_Colored(baseX,baseY - step,m.words,WW,WH);

		step += 16;
	}
}