#pragma once

#include <string>
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
    ECS_DIRECTION,
    ECS_VELOCITY,
    ECS_ENEMY,
    ECS_ANIMATION,
    ECS_2D_BOX_COLLIDER,

    COMPONENT_TYPE_COUNT
};

struct TransformComponent{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct directionComponent{
    glm::vec2 dir;
};

struct VelocityComponent{
    float y;
    float x;
};

struct EnemyComponent{
};

struct SpriteComponent{
    Texture* texture;
    glm::vec2 index = {0, 0};
    glm::vec2 size;

    float layer = 0.0f;
};

struct AnimationComponent{
    std::string id;

    int currentFrame = 0;
    float frameCount = 0;
};

struct InputComponent{
    float x;
    float y;
};

struct Box2DCollider{
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};
};

struct Component{
    Entity id;
    void* data;
};

struct Ecs{
    Entity entities;
    std::unordered_map<ComponentType, std::unordered_map<Entity, Component>> components;
    std::unordered_map<Entity, std::unordered_set<ComponentType>> entityComponentMap;
};

Ecs* initEcs();
uint32_t createEntity(Ecs* ecs, const ComponentType type, const void* data, const size_t size);
void pushComponent(Ecs* ecs, const Entity id, const ComponentType type, const void* data, const size_t size);
void removeComponent(Ecs* ecs, const Entity id, const ComponentType type);
void removeComponents(Ecs* ecs, const Entity id, const std::vector<ComponentType> types);
void removeEntity(Ecs* ecs, const Entity id);
void removeEntities(Ecs* ecs, const std::vector<Entity> entities);
std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents);
void* getComponent(Ecs* ecs, const Entity id, const ComponentType type);
void setComponent(Ecs* ecs, const Entity id, void* data, const ComponentType type);
//void* getCastComponent(Ecs* ecs, Entity id, ComponentType type);
