#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <string>
#include <map>
#include <unordered_map>

extern std::map<std::string, GLuint> allTextures;
extern std::unordered_map<std::string, GLuint> loadedTextures;
extern std::unordered_map<std::string, std::string> texturesNamesAndPaths;

void LoadAllTextures();
GLuint LoadTexture(const char* filename);