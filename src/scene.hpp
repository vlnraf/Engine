#pragma once

#include <glm/glm.hpp>

#include "renderer/renderer.hpp"

struct Model{
    VertexBuffer vertices;
};

struct Object{
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec2 texCoord;
    glm::vec3 color;
    Shader shader;

    Model model;
};

struct Scene{
    Object entities[10];
};

void createScene(Scene* scene);