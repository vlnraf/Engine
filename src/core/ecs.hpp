#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>

#include "renderer/texture.hpp"

#define QUAD_VERTEX_SIZE 30
#define MAX_COMPONENTS 1000

typedef int Entity;
//#define getComponent(ecs, id, type, T) ((T*)getCastComponent(ecs, id, type))

enum ComponentType{
    ECS_TRANSFORM,
    ECS_SPRITE,
    ECS_INPUT,
    ECS_VELOCITY,

    COMPONENT_TYPE_COUNT
};

struct TransformComponent{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct VelocityComponent{
    float y;
    float x;
};

struct SpriteComponent{
    uint32_t id;
    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 

        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f
    };
    uint32_t vertCount;
};

struct InputComponent{
    float x;
    float y;
};

struct Component{
    Entity id;
    void* data;
};

struct Ecs{
    Entity entities;
    std::unordered_map<ComponentType, std::vector<Component>> components;
    std::unordered_map<Entity, std::unordered_set<ComponentType>> entityComponentMap;
};

Ecs* initEcs();
uint32_t createEntity(Ecs* ecs, ComponentType type, void* data, size_t size);
void pushComponent(Ecs* ecs, Entity id, ComponentType type, void* data, size_t size);
void removeComponent(Ecs* ecs, Entity id, ComponentType type);
void removeComponents(Ecs* ecs, Entity id, std::vector<ComponentType> types);
void removeEntity(Ecs* ecs, Entity id);
void removeEntities(Ecs* ecs, std::vector<Entity> entities);
std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents);
void* getComponent(Ecs* ecs, Entity id, ComponentType type);
//void* getCastComponent(Ecs* ecs, Entity id, ComponentType type);
