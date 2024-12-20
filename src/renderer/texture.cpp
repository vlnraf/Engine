#include "texture.hpp"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core/tracelog.hpp"

unsigned char* loadImage(const char* filepath, Texture* texture){
    return stbi_load(filepath, &texture->width, &texture->height, &texture->nrChannels, 0);
}

Texture* loadTexture(const char* filepath){
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
        texture->index = {0,0};
        texture->size = {texture->width, texture->height};
    }else{
        LOGERROR("Errore nel caricamento della texture");
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
        LOGERROR("Errore nel caricamento della texture");
    }

    return texture;
}