#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <string>
#include <map>

extern std::map<std::string, GLuint> allTextures;

void LoadAllTextures();
GLuint LoadTexture(const char* filename);