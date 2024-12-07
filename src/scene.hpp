#pragma once

#include <glm/glm.hpp>


#include "core.hpp"


struct Scene{
    Ecs* ecs;
    TileMap bgMap;
    TileMap fgMap;
};

Scene createScene(Renderer* renderer);
void renderScene(Renderer* renderer, Scene scene);
void updateScene(Input* input, Scene scene, float dt);