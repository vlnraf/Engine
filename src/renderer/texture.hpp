#pragma once

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "core/arena.hpp"
#include "core/coreapi.hpp"

#define MAX_TEXTURES 2056

typedef uint32_t TextureHandle;

struct Texture{
    uint32_t id;
    int width, height, nrChannels;

    //glm::vec2 index;
    glm::vec2 size;
};

struct RenderTexture{
    uint32_t fbo;
    uint32_t rbo;
    Texture texture;
};

struct TextureManager{
    Arena* arena;
    //std::vector<Texture*> textures;
    Texture textures[MAX_TEXTURES];
};

CORE_API void initTextureManager(Arena* arena);
CORE_API TextureHandle loadTexture(const char* fileName);
CORE_API TextureHandle getTextureHandle(const char* fileName);
CORE_API Texture* getTextureByName(const char* fileName);
CORE_API Texture* getTextureByHandle(TextureHandle handle);
CORE_API TextureHandle loadTextureFullPath(const char* path);
CORE_API Texture* getTextureFullPath(const char* path);
CORE_API RenderTexture loadRenderTexture(int width, int height);
CORE_API void destroyRenderTexture(RenderTexture* renderTexture);

//Texture* getTexture(uint32_t idx);
TextureHandle loadFontTexture(const char* path, FT_Face face);
//unsigned char* createTexture(const char* filepath, Texture* texture);
//Texture* loadTexture(const char* filepath);
//Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size);
//Texture* getWhiteTexture();