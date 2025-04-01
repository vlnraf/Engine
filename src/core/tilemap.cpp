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

TileSet createTileSet(Texture* texture, const float tileWidth, const float tileHeight){
    TileSet tileset = {};
    uint32_t colTiles = texture->width / tileWidth;
    uint32_t rowTiles = texture->height / tileHeight;

    //read from bottom to top because different texture coordinates
    //between opengl and std_image
    //the tile at index 0 is the empty tile
    Tile tile = {};
    tileset.tiles.push_back(tile);
    for(int i = 0; i < rowTiles; i++){
        for (int j = 0; j < colTiles; j++){
            Tile tile = createTile(i, j, tileWidth, tileHeight, texture->width, texture->height);
            tileset.tiles.push_back(tile);
        }
    }
    tileset.columns = colTiles;
    tileset.rows = rowTiles;
    tileset.texture = texture;

    return tileset;
}
TileMap LoadTilesetFromTiled(const char* filename){
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
    TileSet tileset = createTileSet(t, ts.tilewidth, ts.tileheight);

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
        //NOTE: Need to be cleared when deleted
        layer.tiles = (int*) malloc(sizeof(l->data) * l->height * l->width);
        memcpy(layer.tiles, l->data, sizeof(l->data) * l->height * l->width);
        map.layers.push_back(layer);
        i++;
        l = l->next;
    }
    cute_tiled_free_map(m);
    return map;
}

void renderTileMap(Renderer* renderer, TileMap map, OrtographicCamera camera){
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

    for(Layer layer : map.layers){
        for(int i = 0; i < layer.mapHeight; i++){
            for(int j = 0; j < layer.mapWidth; j++){
                int tile = layer.tiles[j + (i * layer.mapWidth)];
                if(tile == NO_TILE){ continue; } //The value 0 means no tile placed
                xpos = j * map.tileWidth;
                ypos = (layer.mapHeight * map.tileHeight) - (i * map.tileHeight);
                //tile.ySort = ySort;
                layer.ysort = true;
                renderDrawQuad(renderer, camera, glm::vec3(xpos, ypos, layer.layer),
                                glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                                map.tileset.texture, map.tileset.tiles[tile].index, {map.tileWidth, map.tileHeight}, layer.ysort);
            }
        }
    }

}

//TODO refactor
void renderTileSet(Renderer* renderer, TileSet set, OrtographicCamera camera){
    uint32_t xpos = 0;
    uint32_t ypos = 0;
    uint32_t y = set.rows;

    for(int i = 1; i < set.tiles.size(); i++){
        Tile tile = set.tiles[i];
        xpos = tile.width * ((i-1) % set.columns);
        if(!xpos){
            y--;
            ypos = y * tile.height;
        }
        renderDrawQuad(renderer, camera, glm::vec3(xpos, ypos, 0),
                        glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                        set.texture, tile.index, {tile.width, tile.height}, false);
    }
}