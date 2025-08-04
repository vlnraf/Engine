#include "fontmanager.hpp"
#include "texture.hpp"
#include "core/tracelog.hpp"

#include <glad/glad.h>

#include <ft2build.h>
#include FT_FREETYPE_H  

static FontManager* fontManager;

Font* generateTextureFont(const char* filePath, int characterSize = 48);

int hashFontName(const char* name){
    uint32_t result;
    //cast to unsigned char so i can do math operations on it
    const unsigned char* nameT = (unsigned char*) name;
    const uint32_t multiplier = 97;
    result = nameT[0] * multiplier; //multiply with prime number (reduce collisions)

    for(int i = 1; name[i] != '\0'; i++){
        result = result * multiplier + nameT[i];
    }

    result = result % MAX_FONTS;
    return result;
}

void initFontManager(){
    fontManager = new FontManager();
    memset(fontManager->fonts, 0, sizeof(fontManager->fonts));
    loadFont("Minecraft", 48);
}

void destroyFontManager(){
    for(Font* f : fontManager->fonts){
        if(f){
            delete f->texture;
            delete f;
        }
    }
    delete fontManager;
}

void loadFont(const char* fileName, int characterSize){
    const char* fontPath = "assets/fonts/%s.%s";
    char fullPath[512];
    std::snprintf(fullPath, sizeof(fullPath), fontPath, fileName, "ttf");

    uint32_t hash = hashFontName(fileName);
    if(!fontManager->fonts[hash]){ //NOTE: free the memory of the old texture
        delete fontManager->fonts[hash];
    }
    Font* f = generateTextureFont(fullPath, characterSize);
    f->characterSize = characterSize;
    if(f){
        fontManager->fonts[hash] = f; //NOTE: if a collision occurs i write the new texture on top of the old one!!!
    }else{
        LOGINFO("Unable to generate Texture Font");
    }
    return;
}

Font* getFont(const char* fileName){
    uint32_t hash = hashFontName(fileName);
    return fontManager->fonts[hash];
}

Font* generateTextureFont(const char* filePath, int characterSize){ //Watch the function signature at top for default characterSize
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
       LOGERROR("ERROR::FREETYPE: Could not init FreeType Library");
        return nullptr;
    }

    FT_Face face;
    if (FT_New_Face(ft, filePath, 0, &face))
    {
        LOGERROR("ERROR::FREETYPE: Failed to load font");
        return nullptr;
    }
    FT_Set_Pixel_Sizes(face, 0, characterSize);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        LOGERROR("ERROR::FREETYTPE: Failed to load Glyph");
        return nullptr;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    // generate texture
    Font* font = new Font();
    font->maxHeight = face->size->metrics.height >> 6;

    font->texture = new Texture();
    font->texture->nrChannels = 1;
    glGenTextures(1, &font->texture->id);
    glBindTexture(GL_TEXTURE_2D, font->texture->id);
    for (unsigned char c = 0; c < 128; c++)
    {
        // generate texture
        font->texture->width += face->glyph->bitmap.width;
        font->texture->height = std::max(font->texture->width, (int)face->glyph->bitmap.rows);
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        font->texture->width,
        font->texture->height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    GLint swizzle[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

    int xOffset = 0;

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOGERROR("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
         // Upload glyph to the texture
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            xOffset,
            0,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        Character character = {
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x,
            xOffset // Store the xOffset for UV calculations
        };
        font->characters[c] = character;
        xOffset += face->glyph->bitmap.width;
    }

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return font;
}

uint32_t calculateTextWidth(Font* font, const char* text, float scale){
    uint32_t result = 0;
    for(int i = 0; text[i] != '\0'; i++){
        Character ch = font->characters[(unsigned char) text[i]];
        result += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    return result;
}