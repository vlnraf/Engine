#pragma once

#include <stdint.h>
#include <vector>

#include "renderer/texture.hpp"
#include "renderer/renderer.hpp"

#define QUAD_VERTEX_SIZE 30

struct Tile{
    //float vertices[QUAD_VERTEX_SIZE];
    uint32_t vertCount;
    uint32_t width, height;
    uint32_t xPos, yPos;
    glm::vec2 uvTopLeft;
    glm::vec2 uvBottomRight;
    glm::vec2 index;

    bool ySort = false;
    bool visible;
};


struct TileSet{
    Texture* texture;
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

CORE_API TileMap createTilemap(std::vector<int> tileIdx, const uint32_t width, const uint32_t height, const float tileSize, TileSet tileSet);
CORE_API TileSet createTileSet(Texture* texture, float tileSize);
//void renderTileMap(Renderer* renderer, TileMap map, float layer);
CORE_API void renderTileMap(Renderer* renderer, TileMap map, OrtographicCamera camera, const float layer, const bool ySort);
CORE_API void renderTileSet(Renderer* renderer, TileSet set);
CORE_API std::vector<int> loadTilemapFromFile(const char* filePath, TileSet tileSet, const uint32_t mapWidth);