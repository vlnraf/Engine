#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>

#include "renderer/texture.hpp"

#define MAX_COMPONENTS 1000

typedef uint32_t Entity;
//#define getComponent(ecs, id, type, T) ((T*)getCastComponent(ecs, id, type))

enum ComponentType{
    ECS_TRANSFORM,
    ECS_SPRITE,
    ECS_INPUT,
    ECS_DIRECTION,
    ECS_VELOCITY,
    ECS_PLAYER_TAG,
    ECS_ENEMY_TAG,
    ECS_ANIMATION,
    ECS_2D_BOX_COLLIDER,
    ECS_HITBOX,
    ECS_HURTBOX,
    ECS_ATTACHED_ENTITY,
    ECS_WEAPON,

    ECS_DEBUG_NAME,

    COMPONENT_TYPE_COUNT
};

struct DebugNameComponent{
    std::string name;
};

struct TransformComponent{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct DirectionComponent{
    glm::vec2 dir;
};

struct VelocityComponent{
    glm::vec2 vel;
};

struct EnemyTag{
    int dmg = 10;
    Entity toFollow;
};

struct SpriteComponent{
    enum PivotType {PIVOT_CENTER, PIVOT_BOT_LEFT};
    Texture* texture;
    //std::string texturePath;
    //Can't use string because dynamic memory allocation :(
    //only for deserialize and serialize
    //char texturePath[1024];
    //const char* texturePath;
    //char texturePath[500];
    //To retrieve the texture from the textureManager
    //int textureIndex;
    char textureName[512];
    PivotType pivot = PIVOT_CENTER;
    glm::vec2 index = {0, 0};
    glm::vec2 size;
    glm::vec2 offset = {0, 0};

    bool flipX = false;
    bool flipY = false;

    bool ySort = false;

    float layer = 1.0f;
    bool visible = true;
};

struct AnimationComponent{
    std::string id;
    std::string previousId;

    int currentFrame = 0;
    float frameCount = 0;
    int frames = 0;

    bool loop = true;
};

struct InputComponent{
};

struct PlayerTag{
};

struct WeaponTag{
};

struct AttachedEntity{
    Entity entity;
    glm::vec2 offset;
};

struct Component{
    void* data;
};

struct Ecs{
    Entity entities;
    std::unordered_map<ComponentType, std::unordered_map<Entity, Component>> components;
    std::unordered_map<Entity, std::unordered_set<ComponentType>> entityComponentMap;
};

Ecs* initEcs();
//Entity createEntity(Ecs* ecs, const ComponentType type, const void* data, const size_t size);
Entity createEntity(Ecs* ecs);
Entity createEntity(Ecs* ecs, std::string name, const ComponentType type, const void* data, const size_t size);
void pushComponent(Ecs* ecs, const Entity id, const ComponentType type, const void* data, const size_t size);
void removeComponent(Ecs* ecs, const Entity id, const ComponentType type);
void removeComponents(Ecs* ecs, const Entity id, const std::vector<ComponentType> types);
void removeEntity(Ecs* ecs, const Entity id);
void removeEntities(Ecs* ecs, const std::vector<Entity> entities);
std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents);
void* getComponent(Ecs* ecs, const Entity id, const ComponentType type);
void ecsDestroy(Ecs* ecs);
//void setComponent(Ecs* ecs, const Entity id, void* data, const ComponentType type);
//void* getCastComponent(Ecs* ecs, Entity id, ComponentType type);
