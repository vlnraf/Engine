#pragma once
#include "texture.hpp"
#include "core/coreapi.hpp"

#define MAX_GLYPHS 128 //right now store only the first 128 characters "ascii"
#define MAX_FONTS 1024

struct Character {
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
    int xOffset;
};

struct Font{
    Texture* texture;
    Character characters[MAX_GLYPHS];
    uint32_t maxHeight = 0;
    uint32_t characterSize = 0;
};


struct FontManager{
    //Texture* fontTextures[MAX_FONT_TEXTURES];
    //Character characters[MAX_GLYPHS];
    Font* fonts[MAX_FONTS];
};

CORE_API void initFontManager();
CORE_API void destroyFontManager();
CORE_API void loadFont(const char* fileName, int characterSize = 48);
CORE_API Font* getFont(const char* fileName);
CORE_API uint32_t calculateTextWidth(Font* font, const char* text, float scale);