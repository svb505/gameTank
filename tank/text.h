#pragma once
#include <string>
#include <vector>

extern float timerText;
extern bool showText;
extern std::string g_destroyText;

struct ColoredWord;

void BuildFont();
void PrintText(const char* text);
void RenderTextHUD(float x, float y, float r, float g, float b, const char* text, int screenW, int screenH);
void RenderTextWorld(float x, float y, float z, float r, float g, float b, const char* text);
void showDestroyText(float dt);
void RenderTextHUD_Colored(float x, float y, const std::vector<ColoredWord>& words, int screenW, int screenH);
