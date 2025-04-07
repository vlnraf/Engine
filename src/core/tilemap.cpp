#include "core/tilemap.hpp"
#include "core/tracelog.hpp"

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#define NO_TILE 0

Tile createTile(const uint32_t y, const uint32_t x, const float tileWidth, const float tileHeight, const uint32_t textureWidth, const uint32_t textureHeight){
    Tile tile = {};
    float normTileWidth = (float)tileWidth / textureWidth;
    float normTileHeight = (float)tileHeight / textureHeight;

    float tileLeft = normTileWidth * x;
    float tileRight = normTileWidth * (x + 1);
    float tileBottom = normTileHeight * y;
    float tileTop = normTileHeight * (y + 1);
    tile.uvTopLeft = glm::vec2(tileTop, tileLeft);
    tile.uvBottomRight = glm::vec2(tileBottom, tileRight);
    tile.index = glm::vec2(x, y);

    tile.width = tileWidth;
    tile.height = tileHeight;

    return tile;
}

TileSet createTileSet(cute_tiled_tileset_t* ts, Texture* texture, const float tileWidth, const float tileHeight){
    TileSet tileset = {};
    uint32_t colTiles = texture->width / tileWidth;
    uint32_t rowTiles = texture->height / tileHeight;

    //read from bottom to top because different texture coordinates
    //between opengl and std_image
    //the tile at index 0 is the empty tile
    //Tile tile = {};
    //tileset.tiles.push_back(tile);
    for(int i = 0; i < rowTiles; i++){
        for (int j = 0; j < colTiles; j++){
            Tile tile = createTile(i, j, tileWidth, tileHeight, texture->width, texture->height);
            tile.hasCollider = false;
            tileset.tiles.push_back(tile);
        }
    }
    tileset.columns = colTiles;
    tileset.rows = rowTiles;
    tileset.texture = texture;

    //Load animation on tiles 
    cute_tiled_tile_descriptor_t* tileDesc = ts->tiles;
    while(tileDesc){
        cute_tiled_frame_t* anim =  tileDesc->animation;
        if(anim){
            //Animation tiles
            Tile* tile = &tileset.tiles[anim->tileid];
            int frameCount = tileDesc->frame_count;
            
            tile->animation.frames = frameCount;
            for(int i = 0; i < frameCount; i++){
                tile->animation.frameDuration = (float)anim[i].duration / 1000;
                tile->animation.indices[i] = tileset.tiles[anim[i].tileid].index;
            }
        }
        //Collision tiles
        cute_tiled_layer_t* objectGroup = tileDesc->objectgroup;
        while(objectGroup){
            cute_tiled_object_t* colliders = objectGroup->objects;
            while(colliders){
                Tile* tile = &tileset.tiles[tileDesc->tile_index];
                tile->collider.offset = {colliders->x, tile->height - (colliders->height + colliders->y)};
                tile->collider.size = {colliders->width, colliders->height};
                tile->collider.isTrigger = false;
                tile->collider.type = Box2DCollider::STATIC;
                tile->hasCollider = true;
                colliders = colliders->next;
            }
            objectGroup = objectGroup->next;
        }

        tileDesc = tileDesc->next;
    }

    return tileset;
}
TileMap LoadTilesetFromTiled(const char* filename, Ecs* ecs){
    const char* mapPath = "map/%s.%s";
    const char* extension = "tmj";
    char fullPath[512];

    const char* assetsPath = "%s";
    char imagePath[512];

    std::snprintf(fullPath, sizeof(fullPath), mapPath, filename, extension);

    //NOTE: map imported with cute_tiled lib
    cute_tiled_map_t* m = cute_tiled_load_map_from_file(fullPath, NULL);
    cute_tiled_layer_t* l = m->layers;

    //TileSet creation
    cute_tiled_tileset_t ts = m->tilesets[0];
    std::snprintf(imagePath, sizeof(imagePath), assetsPath, ts.image.ptr + 3); // + 3 to ignore the "../"
    loadTextureFullPath(imagePath);
    Texture* t = getTextureFullPath(imagePath); 
    TileSet tileset = createTileSet(&ts, t, ts.tilewidth, ts.tileheight);

    TileMap map = {};
    int i = 0;
    while(l){
        Layer layer = {};
        //Map creation
        map.tileset = tileset;
        map.tileWidth = m->tilewidth;
        map.tileHeight = m->tileheight;
        layer.layer = (float)i;
        layer.mapHeight = m->layers->height;
        layer.mapWidth = m->layers->width;
        //Tile tile = {};
        //for(int i = 0; i < l->data_count; i++){
        //    layer.tiles.push_back(map.tileset.tiles[l->data[i]]);
        //}
        //NOTE: Need to be cleared when deleted
        layer.tiles = (int*) malloc(sizeof(l->data) * l->data_count);
        memcpy(layer.tiles, l->data, sizeof(l->data) * l->data_count);
        for(int j = 0; j < l->data_count; j++){
            int tileIdx = l->data[j];
            if(tileIdx == 0){ continue; }
            tileIdx -= 1; //NOTE: in the layer the null tile is 0 and each tile start from 1, in the tileset the first tile is the tile at index 0
            if(tileset.tiles[tileIdx].hasCollider){
                Box2DCollider* tileCollider = &tileset.tiles[tileIdx].collider;
                TransformComponent transform = {};
                transform.rotation = {0,0,0};
                transform.scale = {1,1,0};
                float x = (j % layer.mapWidth) * map.tileWidth;
                float y = (layer.mapHeight * map.tileHeight) - (int)(j / layer.mapWidth) * map.tileHeight;
                transform.position = {x, y, 0}; //NOTE: right now push all of them at layer 0
                Entity e = createEntity(ecs);
                pushComponent(ecs, e, TransformComponent, &transform);
                pushComponent(ecs, e, Box2DCollider, tileCollider);
            }
        }
        //Insert only layers with tiles, object layers has no need to be rendered
        if(l->data_count > 0){
            map.layers.push_back(layer);
            i++;
        }
        l = l->next;
    }
    cute_tiled_free_map(m);
    return map;
}

//void updateTileMap(TileMap map, float dt){
//    for(Layer layer : map.layers){
//        for(int i = 0; i < map.tileset.tiles.size(); i++){
//            if(map.tileset.tiles[i].animation.frames != 0){
//                for(int j = 0; j < map.tileset.tiles[i].animation.frames; j++){
//                    layer.tiles[i] = map.tileset.tiles[i].animation.tileIds[j];
//                }
//            }
//        }
//    }
//}

void renderTileMap(Renderer* renderer, TileMap* map, OrtographicCamera camera){
    //if(map.tiles.size() < map.tileWidth * map.tileHeight){
    //    LOGERROR("Non ci sono abbastanza tiles da renderizzare");
    //    exit(0);
    //}
    //if(map.tiles.size() > map.tileWidth * map.tileHeight){
    //    LOGERROR("Ci sono troppe tiles rispetto alla grandezza della mappa");
    //    exit(0);
    //}

    float xpos = 0;
    float ypos = 0;

    for(Layer layer : map->layers){
        for(int i = 0; i < layer.mapHeight; i++){
            for(int j = 0; j < layer.mapWidth; j++){
                int tile = layer.tiles[j + (i * layer.mapWidth)];
                //Tile* tile = &layer.tiles[j + (i * layer.mapWidth)];
                if(tile == NO_TILE){ continue; } //The value 0 means no tile placed
                xpos = j * map->tileWidth;
                ypos = (layer.mapHeight * map->tileHeight) - (i * map->tileHeight);
                //tile.ySort = ySort;
                layer.ysort = true;
                renderDrawQuad(renderer, camera, glm::vec3(xpos, ypos, layer.layer),
                                glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                                map->tileset.texture, map->tileset.tiles[tile-1].index, {map->tileWidth, map->tileHeight}, layer.ysort);
            }
        }
    }

}

//TODO refactor
void renderTileSet(Renderer* renderer, TileSet set, OrtographicCamera camera){
    uint32_t xpos = 0;
    uint32_t ypos = 0;
    uint32_t y = set.rows;

    for(int i = 0; i < set.tiles.size(); i++){
        Tile tile = set.tiles[i];
        xpos = tile.width * (i % set.columns);
        if(!xpos){
            y--;
            ypos = y * tile.height;
        }
        renderDrawQuad(renderer, camera, glm::vec3(xpos, ypos, 0),
                        glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                        set.texture, tile.index, {tile.width, tile.height}, false);
    }
}

void animateTiles(TileMap* map, float dt){
    TileSet* ts = &map->tileset;
    for(int i = 1; i < ts->tiles.size(); i++){
        if(ts->tiles[i].animation.frames != 0){
            //Animate the tile
            ts->tiles[i].animation.elapsedTime += dt;
            if(ts->tiles[i].animation.elapsedTime > ts->tiles[i].animation.frameDuration){
                ts->tiles[i].animation.currentFrame = (ts->tiles[i].animation.currentFrame+1) % ts->tiles[i].animation.frames;
                ts->tiles[i].index = ts->tiles[i].animation.indices[ts->tiles[i].animation.currentFrame];
                ts->tiles[i].animation.elapsedTime = 0;
            }
        };
    }
}