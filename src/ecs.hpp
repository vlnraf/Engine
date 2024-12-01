#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
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
    std::unordered_map<Entity, std::unordered_set<ComponentType>> entityComponentMap;
};

Ecs* initEcs();
void initTransform(Ecs* ecs, Entity id, void* components);
void initSprite(Ecs* ecs, Entity id, void* components);
//void pushComponent(Ecs* ecs, int id, ComponentType type);
void pushComponent(Ecs* ecs, int id, ComponentType type, void* components);
uint32_t createEntity(Ecs* ecs, std::vector<ComponentType> types, std::vector<void*> components);
void updateTranformers(Ecs* ecs, int id, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation);
std::vector<Entity> view(Ecs* ecs, const:: std::vector<ComponentType> requiredComponents);
void removeComponent(Ecs* ecs, int id, std::vector<ComponentType> types);