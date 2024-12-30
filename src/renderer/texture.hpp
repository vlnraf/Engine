#pragma once

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

struct Texture{
    uint32_t id;
    int width, height, nrChannels;

    glm::vec2 index = {0, 0};
    glm::vec2 size;
};

struct TextureManager{
    std::vector<Texture*> textures;
};

TextureManager* initTextureManager();
void destroyTextureManager(TextureManager* textureManager);
int loadTextureInManager(TextureManager* manager, const char* filepath);
Texture* getTexture(TextureManager* manager, size_t index);
unsigned char* loadImage(const char* filepath, Texture* texture);
Texture* loadTexture(const char* filepath);
Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size);
Texture* getWhiteTexture();