#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

typedef int Entity;
typedef char TagName[100];

enum ComponentType{
    ECS_TRANSFORM,
    ECS_HEALTH,
    ECS_INPUT,

    COMPONENT_TYPE_COUNT
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
    //TransformComponent transforms[200];
    //HealthComponent h[200];
    //InputComponent input[200];
    std::vector<TransformComponent> transforms;
    std::vector<HealthComponent> h;
    std::vector<InputComponent> input;
    //std::unordered_map<Entity, TransformComponent> transforms;
    //std::unordered_map<Entity, HealthComponent> h;
    //std::unordered_map<Entity, InputComponent> input;
    TagName tagName;
};

struct Ecs{
    uint32_t maxEntities;

    Entity entities;
    Components components;
};

Ecs* initEcs(uint32_t size);
void initTransform(Ecs* ecs, Entity id);
void initHealth(Ecs* ecs, Entity id);
void initInput(Ecs* ecs, Entity id);
void pushComponent(Ecs* ecs, int id, ComponentType type);
void createEntity(Ecs* ecs, std::vector<ComponentType> types);
void updateTranformers(Ecs* ecs, int id, glm::vec3 pos);
void inputSystem(Ecs* ecs, InputComponent input);