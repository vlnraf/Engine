#pragma once

#include <glm/glm.hpp>

#include "ecs.hpp"
#include "renderer/renderer.hpp"

#define QUAD_VERTEX_SIZE 18

struct Object{
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec2 texCoord;
    glm::vec3 color;
    Shader shader;

    //std::vector<float> vertices;
    float vertices[QUAD_VERTEX_SIZE];
    uint32_t vertCount;
};

struct Scene{
    Ecs* ecs;
    Object entities[10];
};

void createScene(Scene* scene);