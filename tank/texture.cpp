#include <windows.h>
#include <GL/gl.h>
#include "texture.h"
#include "stb_image.h"
#include "Logger.h"
#include <map>
#include <unordered_map>

std::unordered_map<std::string, GLuint> loadedTextures;
std::map<std::string, GLuint> allTextures;
std::unordered_map<std::string, std::string> texturesNamesAndPaths = {
    {"APFSDS","textures/APFSDS.png"},{"HE","textures/HE.png"},{"SMOKE","textures/SMOKE.png"},
    {"ATGM","textures/ATGM.png"},{"SMOKEGR","textures/SMOKE_GRANADE.png"},{"MLRS","textures/mlrs.png"},
    {"ARTILLERY","textures/artillery.png"},{"beton","textures/beton.jpg"},{"betonD","textures/betonD.jpg"},
    {"door","textures/door.jpg"},{"window","textures/window.png"},{"windowDestroyed","textures/windowDestroyed.jpg"},
    {"cloudy","textures/cloudy.jpg"},{"fire","textures/fire.jpg"},{"smoke","textures/smoke.jpg"}
};

void LoadAllTextures() {
    for (auto t : texturesNamesAndPaths) {
        allTextures[t.first] = LoadTexture(t.second.c_str());
    }
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
    else if (channels == 4) format = GL_RGBA;
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