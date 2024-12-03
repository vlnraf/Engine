#include "texture.hpp"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "tracelog.hpp"

void loadImage(const char* filepath, Texture* texture){
    texture->data = stbi_load(filepath, &texture->width, &texture->height, &texture->nrChannels, 0);
}

Texture* loadTexture(const char* filepath){
    Texture* texture = new Texture();
    loadImage(filepath, texture);

    if(texture->data){
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
        glGenerateMipmap(GL_TEXTURE_2D);
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
        whiteTexture->data = white; 
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, whiteTexture->width, whiteTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteTexture->data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return whiteTexture;
}