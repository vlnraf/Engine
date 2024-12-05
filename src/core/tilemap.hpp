#pragma once

#include <stdint.h>
#include <vector>

#include "renderer/texture.hpp"
#include "renderer/renderer.hpp"

#define QUAD_VERTEX_SIZE 30

struct Tile{
    float vertices[QUAD_VERTEX_SIZE];
    uint32_t vertCount;
};


struct TileSet{
    uint32_t id;
    std::vector<Tile> tiles;
};

struct TileMap{
    uint32_t width, height;
    uint32_t tileSize;
    std::vector<Tile> tiles;
    TileSet tileset;
};

TileMap createTilemap(uint32_t width, uint32_t height, uint32_t tileSize);
void renderTileMap(Renderer* renderer, TileMap map);
void renderTileSet(Renderer* renderer, TileSet set);