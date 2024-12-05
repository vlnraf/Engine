#pragma once

#include <stdint.h>

struct Texture{
    uint32_t id;
    int width, height, nrChannels;
};

unsigned char* loadImage(const char* filepath, Texture* texture);
Texture* loadTexture(const char* filepath);
Texture* getWhiteTexture();