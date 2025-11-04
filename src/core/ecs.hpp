#pragma once

#include <glm/glm.hpp>
#include <stdarg.h>

#include "renderer/texture.hpp"
#include "coreapi.hpp"

#define MAX_ENTITIES 50000
#define MAX_COMPONENTS 50000
#define MAX_COMPONENT_TYPE 500

typedef uint32_t Entity;
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

struct EntityArray{
    Entity entities[MAX_ENTITIES];
    size_t count = 0;
};

#define ECS_DECLARE_COMPONENT(TYPE) \
    uint32_t TYPE##_component_id; 

#define ECS_DECLARE_COMPONENT_EXTERN(TYPE) \
    CORE_API uint32_t TYPE##_component_id; 

#define registerComponent(ecs, TYPE)                                \
    TYPE##_component_id = registerComponentImpl(ecs, #TYPE, sizeof(TYPE)); \

#define ECS_TYPE(TYPE) TYPE##_component_id

#define view(ecs, ...) \
    viewImpl(ecs, sizeof((uint32_t[]){__VA_ARGS__}) / sizeof(uint32_t), (uint32_t[]){__VA_ARGS__})

#define getComponent(ecs, e, TYPE) \
    ((TYPE*)getComponentImpl(ecs, e, TYPE##_component_id))

#define removeComponent(ecs, e, TYPE) \
    (removeComponentImpl(ecs, e, TYPE##_component_id))

#define hasComponent(ecs, e, TYPE) \
    (hasComponentImpl(ecs, e, TYPE##_component_id))

#define pushComponent(ecs, e, TYPE, value_ptr) \
    pushComponentImpl(ecs, e, TYPE##_component_id, (const void*)value_ptr)

struct SparseSet{
    size_t entityToComponentCount;
    size_t entityToComponentSize;
    int* entityToComponent;

    Components components;
};

struct DenseToSparse{
    size_t entityCount;
    size_t entitySize;
    int* entity;
};

struct ComponentRegistry{
    size_t componentsCount;
    size_t componentsSize;
    size_t* components;
};


struct Ecs{
    Entity entities;
    Arena* arena;
    SparseSet* sparse;
    DenseToSparse* denseToSparse;

    size_t removedEntitiesCount = 0;
    size_t* removedEntities;
    size_t componentId = 1; // we will use 0 as invalid component
};

CORE_API void importBaseModule(Ecs* ecs);
CORE_API Ecs* initEcs(Arena* arena);
CORE_API Entity createEntity(Ecs* ecs);
CORE_API size_t registerComponentImpl(Ecs* ecs, const char* name, const size_t size);
CORE_API void pushComponentImpl(Ecs* ecs, const Entity id, const size_t componentName, const void* data);
CORE_API bool hasComponentImpl(Ecs* ecs, const Entity entity, const size_t componentName);
CORE_API void* getComponentImpl(Ecs* ecs, Entity entity, const size_t componentName);
CORE_API void removeComponentImpl(Ecs* ecs, Entity entity, const size_t componentName);
CORE_API EntityArray viewImpl(Ecs* ecs, uint32_t count, uint32_t* types);
CORE_API void removeEntity(Ecs* ecs, const Entity entity);
CORE_API void destroyEcs(Ecs* ecs);
CORE_API void clearEcs(Ecs* ecs);

extern ECS_DECLARE_COMPONENT_EXTERN(TransformComponent);
struct TransformComponent{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
};

extern ECS_DECLARE_COMPONENT_EXTERN(DirectionComponent);
struct DirectionComponent{
    glm::vec2 dir;
};

extern ECS_DECLARE_COMPONENT_EXTERN(VelocityComponent);
struct VelocityComponent{
    glm::vec2 vel;
};

extern ECS_DECLARE_COMPONENT_EXTERN(SpriteComponent);
struct SpriteComponent{
    enum PivotType {PIVOT_CENTER, PIVOT_BOT_LEFT};
    Texture* texture;
    char textureName[512] = {0};
    //std::string textureName;
    PivotType pivot = PivotType::PIVOT_BOT_LEFT;// = PIVOT_CENTER;
    glm::vec2 index = {0, 0};
    glm::vec2 size = {0, 0};
    //glm::vec2 offset = {0, 0};
    glm::vec2 tileSize = {0,0};
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

    bool flipX = false;
    bool flipY = false;

    bool ySort = false;

    float layer = 1.0f;
    bool visible = true;
};

extern ECS_DECLARE_COMPONENT_EXTERN(PersistentTag);
struct PersistentTag{};

extern ECS_DECLARE_COMPONENT_EXTERN(AnimationComponent);
struct AnimationComponent{
    char animationId[512];
    char previousId[512];
    //std::string previousId;

    uint16_t currentFrame = 0;
    uint16_t frameCount = 0;
    float elapsedTime = 0;
    //int frames = 0;

    //bool loop = true;
};

extern ECS_DECLARE_COMPONENT_EXTERN(Parent);
struct Parent{
    Entity entity;
};

extern ECS_DECLARE_COMPONENT_EXTERN(Child);
struct Child{
    Entity entity[10]; //NOTE: right now max 10 childs for each entity
    size_t count = 0;
};
