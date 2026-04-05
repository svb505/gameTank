#pragma once

extern float timerText;
extern bool showText;

void BuildFont();
void PrintText(const char* text);
void RenderTextHUD(float x, float y, float r, float g, float b, const char* text, int screenW, int screenH);
void RenderTextWorld(float x, float y, float z, float r, float g, float b, const char* text);
void showDestroyText(int WW, int WH, const char* text, float dt);

