#pragma once

#include <glm/glm.hpp>

#include "core/ecs.hpp"
#include "core/input.hpp"
#include "renderer/renderer.hpp"
#include "core/tracelog.hpp"


struct Scene{
    Ecs* ecs;
};

Scene createScene(Renderer* renderer);
void renderScene(Renderer* renderer, Scene scene);
void updateScene(Input* input, Scene scene, float dt);