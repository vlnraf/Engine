#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "texture.hpp"

#define QUAD_VERTEX_SIZE 30

typedef int Entity;
typedef char TagName[100];

enum ComponentType{
    ECS_TRANSFORM,
    ECS_SPRITE,
    ECS_INPUT,
    ECS_SCRIPT,

    COMPONENT_TYPE_COUNT
};

struct TransformComponent{
    Entity entityId;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct SpriteComponent{
    Entity entityId;
    //Shader shader;
    //uint32_t spriteId;
    Texture* texture;
    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    uint32_t vertCount;
};

struct InputComponent{
    Entity entityId;
    float x;
    float y;
};

struct Components{
    std::vector<TransformComponent> transforms;
    std::vector<SpriteComponent> sprite;
    std::vector<InputComponent> input;
    TagName tagName;
};

struct Ecs{
    Entity entities;
    Components components;
};

Ecs* initEcs();
void initTransform(Ecs* ecs, Entity id);
void initSprite(Ecs* ecs, Entity id);
void initInput(Ecs* ecs, Entity id);
void pushComponent(Ecs* ecs, int id, ComponentType type);
void createEntity(Ecs* ecs, std::vector<ComponentType> types);
void updateTranformers(Ecs* ecs, int id, glm::vec3 pos);
void inputSystem(Ecs* ecs, InputComponent input);