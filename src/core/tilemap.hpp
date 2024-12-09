#pragma once

#include <stdint.h>
#include <vector>

#include "renderer/texture.hpp"
#include "renderer/renderer.hpp"

#define QUAD_VERTEX_SIZE 30

struct Tile{
    float vertices[QUAD_VERTEX_SIZE];
    uint32_t vertCount;
    uint32_t width, height;
    uint32_t xPos, yPos;
    bool visible;
};


struct TileSet{
    uint32_t textureId;
    uint32_t columns;
    uint32_t rows;
    std::vector<Tile> tiles;
};

struct TileMap{
    uint32_t width, height;
    float tileSize;
    std::vector<Tile> tiles;
    TileSet tileset;
};

TileMap createTilemap(std::vector<int> tileIdx, uint32_t width, uint32_t height, float tileSize, TileSet tileSet);
TileSet createTileSet(Texture* texture, float tileSize);
//void renderTileMap(Renderer* renderer, TileMap map, float layer);
void renderTileMap(Renderer* renderer, TileMap map, glm::mat4 view, float layer);
void renderTileSet(Renderer* renderer, TileSet set);
std::vector<int> loadTilemapFromFile(const char* filePath, TileSet tileSet, uint32_t mapWidth);