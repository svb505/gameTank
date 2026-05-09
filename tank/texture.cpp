#include <windows.h>
#include <GL/gl.h>
#include "texture.h"
#include "stb_image.h"
#include "Logger.h"
#include <map>
#include <unordered_map>

std::unordered_map<std::string, GLuint> loadedTextures;
std::map<std::string, GLuint> allTextures;

void LoadAllTextures() {
    allTextures["APFSDS"] = LoadTexture("textures/APFSDS.png");
    allTextures["HE"] = LoadTexture("textures/HE.png");
    allTextures["SMOKE"] = LoadTexture("textures/SMOKE.png");
    allTextures["ATGM"] = LoadTexture("textures/ATGM.png");
    allTextures["SMOKEGR"] = LoadTexture("textures/SMOKE_GRANADE.png");
    allTextures["MLRS"] = LoadTexture("textures/mlrs.png");
    allTextures["ARTILLERY"] = LoadTexture("textures/artillery.png");
    allTextures["beton"] = LoadTexture("textures/beton.jpg");
    allTextures["betonD"] = LoadTexture("textures/betonD.jpg");
    allTextures["window"] = LoadTexture("textures/window.png");
    allTextures["windowDestroyed"] = LoadTexture("textures/windowDestroyed.jpg");
    allTextures["door"] = LoadTexture("textures/door.png");
    allTextures["cloudy"] = LoadTexture("textures/cloudy.jpg");
    allTextures["fire"] = LoadTexture("textures/fire.jpg");
    allTextures["smoke"] = LoadTexture("textures/smoke.jpg");
    allTextures["tank"] = LoadTexture("textures/tank.jpg");
}
GLuint LoadTexture(const char* filename){
    if (loadedTextures.contains(filename)) return loadedTextures[filename];

    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data){
        LOG_ERROR("Failed to load image");
        return 0;
    }

    GLenum format = GL_RGB;

    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4)format = GL_RGBA;
    else{
        LOG_ERROR("Unsupported image format");
        stbi_image_free(data);
        return 0;
    }

    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);

    stbi_image_free(data);

    loadedTextures[filename] = texture;

    return texture;
}