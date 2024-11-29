#pragma once

#include <stdint.h>

struct Texture{
    uint32_t id;
    int width, height, nrChannels;
    uint8_t* data;
};

void loadImage(const char* filepath, Texture* texture);
Texture* loadTexture(const char* filepath);