#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>
#include <stdarg.h>

#include "renderer/texture.hpp"
#include "coreapi.hpp"

//#define MAX_COMPONENTS 1000
#define MAX_ENTITIES 1000
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
    Entity toFollow;
};

struct SpriteComponent{
    enum PivotType {PIVOT_CENTER, PIVOT_BOT_LEFT};
    Texture* texture;
    char textureName[512];
    PivotType pivot = PIVOT_CENTER;
    glm::vec2 index = {0, 0};
    glm::vec2 size;
    glm::vec2 offset = {0, 0};
    glm::vec4 color = {1,1,1,1};

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

//TODO: preallocate a vector for each type of component and store data in there to have contigous memory
struct Component{
    //Entity entity;
    void* data;
};

struct Components{
    void* elements;
    size_t count;
    size_t elementSize;
};

//using Column = std::vector<T>;

#define registerComponent(ecs, T) registerComponentName(ecs, #T, sizeof(T))
#define pushComponent(ecs, entity, T, data) pushComponentName(ecs, entity, #T, data, sizeof(T))
#define hasComponent(ecs, entity, T) hasComponentName(ecs, entity, #T)
#define getComponent(ecs, entity, T) ((T*) getComponentName(ecs, entity, #T))
#define getComponentVector(ecs, T) ((T*) getComponentVectorName(ecs, #T))
#define removeComponent(ecs, entity, T) removeComponentName(ecs, entity, #T)
//#define view(ecs, ...) viewName(ecs, {#__VA_ARGS__})
#define view(ecs, ...) viewName(ecs, #__VA_ARGS__)


struct Ecs{
    Entity entities;
    
    //std::unordered_map<ComponentType, std::unordered_map<Entity, Component>> components;
    //std::unordered_map<Entity, std::unordered_set<ComponentType>> entityComponentMap;


    //sparse vector
    //std::unordered_map<ComponentType, std::vector<size_t>> sparse;
    std::unordered_map<size_t, std::vector<int>> sparse;
    //dense vectors
    //std::unordered_map<ComponentType, std::vector<Component>> dense;
    //std::unordered_map<ComponentType, Components> dense;
    std::unordered_map<size_t, Components> dense;

    //std::unordered_map<ComponentType, std::vector<size_t>> denseToSparse;
    std::unordered_map<size_t, std::vector<size_t>> denseToSparse;

    size_t componentId = 1; // we will use 0 as invalid component
    std::unordered_map<std::string, size_t> componentRegistry;

    std::vector<size_t> removedEntities;
};

CORE_API Ecs* initEcs();
//Entity createEntity(Ecs* ecs, const ComponentType type, const void* data, const size_t size);
CORE_API Entity createEntity(Ecs* ecs);
//CORE_API bool hasComponent(Ecs* ecs, const Entity entity, const ComponentType type);
//CORE_API void registerComponent(Ecs* ecs, ComponentType type, size_t size);
CORE_API void registerComponentName(Ecs* ecs, const char* componentName, const size_t size);
CORE_API void pushComponentName(Ecs* ecs, const Entity id, const char* componentName, const void* data, const size_t size);
CORE_API bool hasComponentName(Ecs* ecs, const Entity entity, const char* componentName);
CORE_API void* getComponentName(Ecs* ecs, Entity entity, const char* componentName);
CORE_API void* getComponentVectorName(Ecs* ecs, const char* componentName);
CORE_API void removeComponentName(Ecs* ecs, Entity entity, const char* componentName);
CORE_API std::vector<Entity> viewName(Ecs* ecs, ...);
//CORE_API void* getComponentVector(Ecs* ecs, ComponentType type);
//CORE_API std::vector<std::vector<Component>> viewComponents(Ecs* ecs, std::vector<ComponentType> types);
//Entity createEntity(Ecs* ecs, std::string name, const ComponentType type, const void* data, const size_t size);
//CORE_API void pushComponent(Ecs* ecs, const Entity id, const ComponentType type, const void* data, const size_t size);
//CORE_API void removeComponent(Ecs* ecs, const Entity id, const ComponentType type);
//CORE_API void removeComponents(Ecs* ecs, const Entity id, const std::vector<ComponentType> types);
CORE_API void removeEntity(Ecs* ecs, const Entity entity);
//CORE_API void removeEntities(Ecs* ecs, const std::vector<Entity> entities);
//CORE_API std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents);
//CORE_API void* getComponent(Ecs* ecs, const Entity entity, const ComponentType type);
CORE_API void ecsDestroy(Ecs* ecs);
//void setComponent(Ecs* ecs, const Entity id, void* data, const ComponentType type);
//void* getCastComponent(Ecs* ecs, Entity id, ComponentType type);



CORE_API void* back(Components* components);
CORE_API void pop_back(Components* components);
CORE_API void push_back(Components* components, const void* data);
CORE_API void insert(Components* components, size_t index, const void* data);
CORE_API Components initComponents(size_t size);
CORE_API void* get(Components* components, size_t index);