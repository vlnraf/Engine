#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

struct Texture{
    uint32_t id;
    int width, height, nrChannels;

    glm::vec2 index = {0, 0};
    glm::vec2 size;
};

unsigned char* loadImage(const char* filepath, Texture* texture);
Texture* loadTexture(const char* filepath);
Texture* loadSubTexture(const char* filepath, glm::vec2 index, glm::vec2 size);
Texture* getWhiteTexture();