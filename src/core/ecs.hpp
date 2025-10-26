#pragma once

//#include <string>
//#include <vector>
//#include <unordered_map>
//#include <unordered_set>
#include <glm/glm.hpp>
#include <stdarg.h>

#include "renderer/texture.hpp"
#include "coreapi.hpp"

//#define MAX_COMPONENTS 1000
#define MAX_ENTITIES 5000
#define MAX_COMPONENTS 5000
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


//#define registerComponent(ecs, T)           \
//    static size_t T##Id = 0;           \
//    static size_t get##T##Id(void) {    \
//        if (name##Id < 1) {             \
//            name##Id = registerComponentName(ecs, #T, sizeof(T)); \
//        }                               \
//        return name##Id;                \
//    }
//
//#define getComponentId(T) get##T##Id()
//
////#define registerComponent(ecs, T) registerComponentName(ecs, #T, sizeof(T))
//#define pushComponent(ecs, entity, T, data) pushComponentName(ecs, entity, getComponentId(T), data)
//#define hasComponent(ecs, entity, T) hasComponentName(ecs, entity, getComponentId(T))
//#define getComponent(ecs, entity, T) ((T*) getComponentName(ecs, entity, getComponentId(T)))
////#define getComponentVector(ecs, T) ((T*) getComponentVectorName(ecs, #T))
//#define removeComponent(ecs, entity, T) removeComponentName(ecs, entity, getComponentId(T))
////#define view(ecs, ...) viewName(ecs, {#__VA_ARGS__})
//#define view(ecs, ...) viewName(ecs, #__VA_ARGS__)

//#define ECS_DECLARE_COMPONENT(TYPE) \
//    int TYPE##_component_id = 0; 


#define ECS_DECLARE_COMPONENT(TYPE) \
    uint32_t TYPE##_component_id; 

#define ECS_DECLARE_COMPONENT_EXTERN(TYPE) \
    CORE_API uint32_t TYPE##_component_id; 

#define registerComponent(ecs, TYPE)                                \
    TYPE##_component_id = registerComponentImpl(ecs, #TYPE, sizeof(TYPE)); \

#define ECS_TYPE(TYPE) TYPE##_component_id

//#define view(ecs, q) \
//    viewImpl(ecs, sizeof(q) /sizeof(uint32_t), q)

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

    //size_t componentsCount;
    //size_t componentsSize;
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

    //std::unordered_map<size_t, std::vector<int>> sparse;
    //std::unordered_map<size_t, Components> dense;
    //std::unordered_map<size_t, std::vector<size_t>> denseToSparse;
    //size_t componentId = 1; // we will use 0 as invalid component
    //std::unordered_map<std::string, size_t> componentRegistry;
    //std::vector<size_t> removedEntities;

    Arena* arena;
    SparseSet* sparse;
    //DenseSet* dense;
    DenseToSparse* denseToSparse;
    //ComponentRegistry* componentRegistry;

    size_t removedEntitiesCount = 0;
    size_t* removedEntities;
    char names[MAX_COMPONENT_TYPE][500];
    //static size_t count = 1;
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



//CORE_API void* back(Components* components);
//CORE_API void pop_back(Components* components);
//CORE_API void push_back(Components* components, const void* data);
//CORE_API void insert(Components* components, size_t index, const void* data);
//CORE_API Components initComponents(Arena* arena, size_t size);
//CORE_API void* get(Components* components, size_t index);

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

