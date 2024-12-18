#pragma once

#include <glm/glm.hpp>


#include "core.hpp"
#include "kit.hpp"

//struct AnimationManager{
//    std::unordered_map<std::string, AnimationComponent> animations;
//};

struct Scene{
    Ecs* ecs;
    OrtographicCamera camera;
    Entity player;
    TileMap bgMap;
    TileMap fgMap;
    //AnimationManager animationManager;
};

Scene createScene(Renderer* renderer);
void renderScene(Renderer* renderer, Scene* scene, float dt);
void updateScene(Input* input, Scene* scene, float dt);