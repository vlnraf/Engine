#include "texture.hpp"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core/tracelog.hpp"

Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size);
Texture* createTexture(const char* filepath);
Texture* getWhiteTexture();

int hashTextureName(const char* name){
    uint32_t result;
    //cast to unsigned char so i can do math operations on it
    const unsigned char* nameT = (unsigned char*) name;
    const uint32_t multiplier = 97;
    result = nameT[0] * multiplier; //multiply with prime number (reduce collisions)

    for(int i = 1; name[i] != '\0'; i++){
        result = result * multiplier + nameT[i];
    }

    result = result % MAX_TEXTURES;
    return result;
}

TextureManager* initTextureManager(){
    TextureManager* manager = new TextureManager();
    memset(manager->textures, 0, sizeof(manager->textures));
    Texture* whiteTexture = getWhiteTexture();
    uint32_t hash = hashTextureName("Default Texture");
    manager->textures[hash] = whiteTexture;
    //manager->textures.push_back(whiteTexture);
    return manager;
}

void destroyTextureManager(TextureManager* manager){
    for(Texture* t : manager->textures){
        delete t;
    }
    delete manager;
}

void loadTexture(TextureManager* manager, const char* fileName){
    const char* assetsPath = "assets/sprites/%s.%s";
    char fullPath[512];
    std::snprintf(fullPath, sizeof(fullPath), assetsPath, fileName, "png");

    uint32_t hash = hashTextureName(fileName);
    if(!manager->textures[hash]){ //NOTE: free the memory of the old texture
        delete manager->textures[hash];
    }
    Texture* t = createTexture(fullPath);
    manager->textures[hash] = t; //NOTE: if a collision occurs i write the new texture on top of the old one!!!
    //manager->textures.push_back(t);
    //return manager->textures.size()-1;
}

Texture* getTexture(TextureManager* manager, const char* fileName){
    uint32_t hash = hashTextureName(fileName);
    return manager->textures[hash];
    //return manager->textures[index];
}

unsigned char* loadImage(const char* filePath, Texture* texture){
    return stbi_load(filePath, &texture->width, &texture->height, &texture->nrChannels, 0);
}

Texture* createTexture(const char* filePath){
    Texture* texture = new Texture();
    unsigned char* data = loadImage(filePath, texture);

    if(data){
        GLenum format;
        switch(texture->nrChannels){
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
        }

        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_2D, texture->id);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        texture->index = {0,0};
        texture->size = {texture->width, texture->height};
    }else{
        delete texture;
        LOGERROR("Errore nel caricamento della texture");
        return nullptr;
    }

    return texture;
}

Texture* getWhiteTexture(){
    static Texture* whiteTexture = nullptr;  // Static variable to store the white texture
    // If the white texture hasn't been created yet, create it
    if (whiteTexture == nullptr) {
        whiteTexture = new Texture();
        static uint8_t white[4] = {255, 255, 255, 255};
        whiteTexture->width = 1;
        whiteTexture->height = 1;
        whiteTexture->nrChannels = 4;

        glGenTextures(1, &whiteTexture->id);
        glBindTexture(GL_TEXTURE_2D, whiteTexture->id);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, whiteTexture->width, whiteTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glGenerateMipmap(GL_TEXTURE_2D);
        whiteTexture->index = {0,0};
        whiteTexture->size = {whiteTexture->width, whiteTexture->height};
    }

    return whiteTexture;
}

Texture* getTransparentTexture(){
    static Texture* whiteTexture = nullptr;  // Static variable to store the white texture
    // If the white texture hasn't been created yet, create it
    if (whiteTexture == nullptr) {
        whiteTexture = new Texture();
        static uint8_t white[4] = {255, 255, 255, 0};
        whiteTexture->width = 1;
        whiteTexture->height = 1;
        whiteTexture->nrChannels = 4;

        glGenTextures(1, &whiteTexture->id);
        glBindTexture(GL_TEXTURE_2D, whiteTexture->id);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, whiteTexture->width, whiteTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glGenerateMipmap(GL_TEXTURE_2D);
        whiteTexture->index = {0,0};
        whiteTexture->size = {whiteTexture->width, whiteTexture->height};
    }

    return whiteTexture;
}

Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size){
    Texture* texture = new Texture();
    unsigned char* data = loadImage(filepath, texture);

    if(data){
        GLenum format;
        switch(texture->nrChannels){
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
        }

        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_2D, texture->id);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        texture->index = index;
        texture->size = size;
    }else{
        delete texture;
        LOGERROR("Errore nel caricamento della texture");
        return nullptr;
    }

    return texture;
}
