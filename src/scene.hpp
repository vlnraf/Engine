#pragma once

#include <glm/glm.hpp>

#include "ecs.hpp"
#include "renderer/renderer.hpp"
#include "tracelog.hpp"


struct Scene{
    Ecs* ecs;
};

void createScene(Scene* scene, Renderer* renderer);
void renderScene(Scene* scene, Renderer* renderer);