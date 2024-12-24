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
    ECS_HEALTH,
    ECS_WEAPON,

    ECS_DEBUG_NAME,

    COMPONENT_TYPE_COUNT
};

struct DebugNameComponent{
    const std::string name;
};

struct TransformComponent{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct DirectionComponent{
    glm::vec2 dir;
};

struct VelocityComponent{
    glm::vec2 vel;
};

struct EnemyTag{
    float dmg = 10.0f;
};

struct HealthComponent{
    int value;
};

struct SpriteComponent{
    enum PivotType {PIVOT_CENTER, PIVOT_BOT_LEFT};
    Texture* texture;
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
    float dmg = 10.0f;
};

struct Box2DCollider{
    //TODO: phisics body instead of do in in collider???
    enum ColliderType {DYNAMIC, STATIC};
    ColliderType type;
    bool active = true;
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};

    bool onCollision = false;
    //TODO: new component that manage hitboxes like: hitBoxComponent and hurtBoxComponent
    //bool hitted = false;
};

struct HitBox{
    Box2DCollider area;
    bool hit = false;
    //TODO: implement
};

struct HurtBox{
    Box2DCollider area;
    bool hit = false;
    //TODO: implement
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
Entity createEntity(Ecs* ecs, std::string name, const ComponentType type, const void* data, const size_t size);
void pushComponent(Ecs* ecs, const Entity id, const ComponentType type, const void* data, const size_t size);
void removeComponent(Ecs* ecs, const Entity id, const ComponentType type);
void removeComponents(Ecs* ecs, const Entity id, const std::vector<ComponentType> types);
void removeEntity(Ecs* ecs, const Entity id);
void removeEntities(Ecs* ecs, const std::vector<Entity> entities);
std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents);
void* getComponent(Ecs* ecs, const Entity id, const ComponentType type);
//void setComponent(Ecs* ecs, const Entity id, void* data, const ComponentType type);
//void* getCastComponent(Ecs* ecs, Entity id, ComponentType type);
