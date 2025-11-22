#include "core/tilemap.hpp"
#include "core/tracelog.hpp"
#include "game/components.hpp"

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#define NO_TILE 0

Tile createTile(const uint32_t y, const uint32_t x, const float tileWidth, const float tileHeight, const uint32_t textureWidth, const uint32_t textureHeight){
    Tile tile = {};
    tile.index = glm::vec2(x, y);
    tile.width = tileWidth;
    tile.height = tileHeight;
    tile.sourceRect = {
        .pos = {x * tileWidth, y * tileHeight},
        .size = {tileWidth, tileHeight}
    };

    // Calculate normalized UV coordinates (legacy support)
    float normTileWidth = tileWidth / textureWidth;
    float normTileHeight = tileHeight / textureHeight;
    tile.uvTopLeft = {normTileWidth * x, normTileHeight * (y + 1)};
    tile.uvBottomRight = {normTileWidth * (x + 1), normTileHeight * y};

    return tile;
}

TileSet createTileSet(cute_tiled_tileset_t* ts, Texture* texture, const float tileWidth, const float tileHeight){
    TileSet tileset = {};
    tileset.texture = texture;
    tileset.columns = texture->width / tileWidth;
    tileset.rows = texture->height / tileHeight;

    // Create tiles grid
    for(uint32_t row = 0; row < tileset.rows; row++){
        for(uint32_t col = 0; col < tileset.columns; col++){
            Tile tile = createTile(row, col, tileWidth, tileHeight, texture->width, texture->height);
            tile.hasCollider = false;
            tileset.tiles.push_back(tile);
        }
    }

    // Load tile properties and animations
    for(cute_tiled_tile_descriptor_t* tileDesc = ts->tiles; tileDesc; tileDesc = tileDesc->next){
        Tile* tile = &tileset.tiles[tileDesc->tile_index];

        // Load custom tile properties (e.g., ySortOffset)
        for(int i = 0; i < tileDesc->property_count; i++){
            if(strcmp(tileDesc->properties[i].name.ptr, "ySortOffset") == 0){
                if(tileDesc->properties[i].type == CUTE_TILED_PROPERTY_FLOAT){
                    tile->ySortOffset = tileDesc->properties[i].data.floating;
                } else if(tileDesc->properties[i].type == CUTE_TILED_PROPERTY_INT){
                    tile->ySortOffset = (float)tileDesc->properties[i].data.integer;
                }
            }
        }

        if(tileDesc->animation){
            Animation* anim = &tile->animation;

            anim->frames = tileDesc->frame_count;
            anim->tileSize = {tileWidth, tileHeight};

            for(int i = 0; i < tileDesc->frame_count; i++){
                anim->frameDuration = (float)tileDesc->animation[i].duration / 1000.0f;
                anim->indices[i] = tileset.tiles[tileDesc->animation[i].tileid].index;
            }
        }

        // Load tile colliders
        for(cute_tiled_layer_t* objectGroup = tileDesc->objectgroup; objectGroup; objectGroup = objectGroup->next){
            for(cute_tiled_object_t* collider = objectGroup->objects; collider; collider = collider->next){
                Tile* tile = &tileset.tiles[tileDesc->tile_index];
                tile->collider.offset = {
                    (collider->x + ((collider->width - tile->width) + collider->x)) * 0.5f,
                    -(collider->y + ((collider->height - tile->height) + collider->y)) * 0.5f
                };
                tile->collider.size = {collider->width, collider->height};
                tile->collider.isTrigger = false;
                tile->collider.type = Box2DCollider::STATIC;
                tile->hasCollider = true;
            }
        }
    }

    return tileset;
}

TileMap LoadTilesetFromTiled(const char* filename, Ecs* ecs){
    char fullPath[512];
    char imagePath[512];

    std::snprintf(fullPath, sizeof(fullPath), "map/%s.tmj", filename);

    // Load map from Tiled file
    cute_tiled_map_t* m = cute_tiled_load_map_from_file(fullPath, NULL);

    // Load tileset texture
    cute_tiled_tileset_t ts = m->tilesets[0];
    std::snprintf(imagePath, sizeof(imagePath), "%s", ts.image.ptr + 3); // +3 to skip "../"
    loadTextureFullPath(imagePath);
    Texture* texture = getTextureFullPath(imagePath);

    // Create tilemap
    TileMap map = {};
    map.tileset = createTileSet(&ts, texture, ts.tilewidth, ts.tileheight);
    map.tileWidth = m->tilewidth;
    map.tileHeight = m->tileheight;

    // Load layers
    int layerIndex = 0;
    for(cute_tiled_layer_t* l = m->layers; l; l = l->next){
        if(l->data_count == 0) continue; // Skip object layers

        Layer layer = {};
        layer.layer = (float)layerIndex;
        layer.mapWidth = m->layers->width;
        layer.mapHeight = m->layers->height;

        layer.ysort = false;
        if(layerIndex == 1){
            layer.ysort = true;
        }

        layer.tiles = (int*)malloc(sizeof(l->data) * l->data_count);
        memcpy(layer.tiles, l->data, sizeof(l->data) * l->data_count);

        // Create collider entities for tiles with colliders
        for(int j = 0; j < l->data_count; j++){
            int tileIdx = l->data[j];
            if(tileIdx == 0) continue; // Skip empty tiles

            tileIdx -= 1; // Tiled uses 1-based indexing, we use 0-based

            if(map.tileset.tiles[tileIdx].hasCollider){
                TransformComponent transform = {
                    .position = {
                        (j % layer.mapWidth) * map.tileWidth,
                        (layer.mapHeight * map.tileHeight) - (j / layer.mapWidth) * map.tileHeight,
                        0
                    },
                    .scale = {1, 1, 1},
                    .rotation = {0, 0, 0}
                };

                Entity e = createEntity(ecs);
                pushComponent(ecs, e, TransformComponent, &transform);
                pushComponent(ecs, e, Box2DCollider, &map.tileset.tiles[tileIdx].collider);
            }
        }

        //map.layers.push_back(layer);
        if(l->data_count > 0){
            map.layers.push_back(layer);
            layerIndex++;
        }
    }

    cute_tiled_free_map(m);
    return map;
}

void renderTileMap(TileMap* map){
    for(const Layer& layer : map->layers){
        for(uint32_t row = 0; row < layer.mapHeight; row++){
            for(uint32_t col = 0; col < layer.mapWidth; col++){
                int tileIdx = layer.tiles[col + (row * layer.mapWidth)];
                if(tileIdx == NO_TILE) continue;

                float xpos = col * map->tileWidth;
                float ypos = (layer.mapHeight * map->tileHeight) - (row * map->tileHeight);

                const Tile& tile = map->tileset.tiles[tileIdx - 1];

                renderDrawQuadPro(
                    {xpos, ypos, layer.layer},
                    {map->tileWidth, map->tileHeight},
                    {0.0f, 0.0f, 0.0f},
                    tile.sourceRect,
                    {0.5f, 0.5f},
                    map->tileset.texture,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    layer.ysort,
                    tile.ySortOffset  // Pass tile-specific y-sort offset
                );
            }
        }
    }
}

void renderTileSet(TileSet set){
    uint32_t y = set.rows;

    for(size_t i = 0; i < set.tiles.size(); i++){
        const Tile& tile = set.tiles[i];
        uint32_t xpos = tile.width * (i % set.columns);
        uint32_t ypos = y * tile.height;

        if(xpos == 0) y--;

        renderDrawQuadEx(
            {xpos, ypos, 0},
            {(float)tile.width, (float)tile.height},
            {0.0f, 0.0f, 0.0f},
            set.texture,
            tile.sourceRect,
            {1.0f, 1.0f, 1.0f, 1.0f}
        );
    }
}

void animateTiles(TileMap* map, float dt){
    TileSet* ts = &map->tileset;
    for(size_t i = 1; i < ts->tiles.size(); i++){
        Tile* tile = &ts->tiles[i];
        Animation* anim = &tile->animation;

        if(anim->frames == 0) continue;

        anim->elapsedTime += dt;
        if(anim->elapsedTime > anim->frameDuration){
            anim->currentFrame = (anim->currentFrame + 1) % anim->frames;
            glm::vec2 frameIndex = anim->indices[anim->currentFrame];

            tile->index = frameIndex;
            tile->sourceRect = gridToPixelRect(frameIndex, anim->tileSize);
            anim->elapsedTime = 0;
        }
    }
}
