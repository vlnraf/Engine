#pragma once

#include <glm/glm.hpp>

typedef int Entity;

enum ComponentType{
    TRANSFORM,
    HEALTH,
    INPUT
};

struct TransformComponent{
    Entity entityId;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct HealthComponent{
    Entity entityId;
    uint32_t hp;
};

struct InputComponent{
    Entity entityId;
    float x;
    float y;
};

struct Components{
    TransformComponent transforms[200];
    HealthComponent h[200];
    InputComponent input[200];
};

struct Ecs{
    uint32_t maxEntities;

    Entity entities;
    Components components;
};