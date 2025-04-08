#pragma once

#include <stdint.h>
#include <vector>

#include "renderer/texture.hpp"
#include "renderer/renderer.hpp"
#include "core/animationmanager.hpp"
#include "core/ecs.hpp"
#include "core/colliders.hpp"

#define QUAD_VERTEX_SIZE 30

struct Tile{
    //float vertices[QUAD_VERTEX_SIZE];
    //uint32_t vertCount;
    uint16_t tileId;
    uint32_t width, height;
    uint32_t xPos, yPos;
    glm::vec2 uvTopLeft;
    glm::vec2 uvBottomRight;
    glm::vec2 index;

    bool ySort = false;
    bool visible;
    Animation animation;
    bool hasCollider = false;
    Box2DCollider collider;
};


struct TileSet{
    Texture* texture;
    uint32_t columns;
    uint32_t rows;
    std::vector<Tile> tiles;
};

struct Layer{
    float layer;
    uint32_t mapWidth, mapHeight;
    //std::vector<Tile> tiles;
    int* tiles;
    bool ysort;
};

struct TileMap{
    //uint32_t width, height;
    float tileWidth;
    float tileHeight;
    //std::vector<Tile> tiles;
    std::vector<Layer> layers;
    TileSet tileset;
};

CORE_API TileMap createTilemap(std::vector<int> tileIdx, const uint32_t width, const uint32_t height, const float tileSize, TileSet tileSet);
CORE_API TileSet createTileSet(Texture* texture, const float tileWidth, const float tileHeight);
//void renderTileMap(Renderer* renderer, TileMap map, float layer);
CORE_API void renderTileMap(TileMap* map);
CORE_API void renderTileSet(TileSet set);
CORE_API std::vector<int> loadTilemapFromFile(const char* filePath, TileSet tileSet, const uint32_t mapWidth);

CORE_API TileMap LoadTilesetFromTiled(const char* filename, Ecs* ecs);
CORE_API void animateTiles(TileMap* map, float dt);