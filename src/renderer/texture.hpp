#pragma once

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "core/arena.hpp"
#include "core/coreapi.hpp"

#define MAX_TEXTURES 2056

struct Texture{
    uint32_t id;
    int width, height, nrChannels;

    glm::vec2 index = {0, 0};
    glm::vec2 size;
};

struct TextureManager{
    Arena* arena;
    //std::vector<Texture*> textures;
    Texture* textures[MAX_TEXTURES];
};

CORE_API void initTextureManager(Arena* arena);
CORE_API void loadTexture(const char* fileName);
CORE_API Texture* getTexture(const char* fileName);
Texture* getTexture(uint32_t idx);
CORE_API void loadTextureFullPath(const char* path);
CORE_API Texture* getTextureFullPath(const char* path);
uint32_t loadFontTexture(const char* path, FT_Face face);
//unsigned char* createTexture(const char* filepath, Texture* texture);
//Texture* loadTexture(const char* filepath);
//Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size);
//Texture* getWhiteTexture();