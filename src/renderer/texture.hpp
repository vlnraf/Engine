#pragma once

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

#define MAX_TEXTURES 2056

struct Texture{
    uint32_t id;
    int width, height, nrChannels;

    glm::vec2 index = {0, 0};
    glm::vec2 size;
};

struct TextureManager{
    //std::vector<Texture*> textures;
    Texture* textures[MAX_TEXTURES];
};

TextureManager* initTextureManager();
void destroyTextureManager(TextureManager* textureManager);
void loadTexture(TextureManager* manager, const char* fileName);
Texture* getTexture(TextureManager* manager, const char* fileName);
//unsigned char* createTexture(const char* filepath, Texture* texture);
//Texture* loadTexture(const char* filepath);
//Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size);
//Texture* getWhiteTexture();