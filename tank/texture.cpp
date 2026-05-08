#include <windows.h>
#include <GL/gl.h>
#include "texture.h"
#include "stb_image.h"
#include "Logger.h"
#include <map>

std::map<std::string, GLuint> iconTextures;

void LoadAllTextures() {
    iconTextures["APFSDS"] = LoadTexture("icons/APFSDS.png");
    iconTextures["HE"] = LoadTexture("icons/HE.png");
    iconTextures["SMOKE"] = LoadTexture("icons/SMOKE.png");
    iconTextures["ATGM"] = LoadTexture("icons/ATGM.png");
    iconTextures["SMOKEGR"] = LoadTexture("icons/SMOKE_GRANADE.png");
    iconTextures["MLRS"] = LoadTexture("icons/mlrs.png");
    iconTextures["ARTILLERY"] = LoadTexture("icons/artillery.png");
}
GLuint LoadTexture(const char* filename){
    int width, height, channels;

    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data){
        LOG_ERROR("Failed to load image");
        return 0;
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

    stbi_image_free(data);

    return texture;
}