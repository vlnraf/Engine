#pragma once
#include "core/ecs.hpp"
#include "core/coreapi.hpp"

extern ECS_DECLARE_COMPONENT_EXTERN(Box2DCollider)
struct Box2DCollider{
    //TODO: phisics body instead of do in in collider???
    enum ColliderType {DYNAMIC, STATIC};
    ColliderType type;
    //bool active = true;
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};
    glm::vec2 relativePosition = {0,0};
    bool isTrigger = false;
};

enum CollisionType{
    PHYSICS,
    TRIGGER
};

struct EntityCollider{
    Entity entity;
    Box2DCollider* collider;
};

struct CollisionEvent{
    EntityCollider entityA;
    EntityCollider entityB;
    CollisionType type;
};

struct CollisionEventArray{
    CollisionEvent* item;
    size_t count = 0;
};

struct TriggerEventArray{
    CollisionEvent* item;
    size_t count = 0;
};

struct EntityColliderArray{
    EntityCollider* item;
    size_t count = 0;
};

struct CollisionGrid{
    float originX, originY = 0;
    float centerX, centerY = 0;

    EntityColliderArray* cell;
};

struct HashSet{
    uint64_t* event;
    bool* occupied;
};

struct CollisionManager{
    CollisionEventArray collisionEvents;
    TriggerEventArray triggerEvents;
    HashSet currEvents;
    HashSet prevEvents;
    TriggerEventArray triggerEventsPrev;
    TriggerEventArray triggerEnterEvents;
    TriggerEventArray triggerExitEvents;
    CollisionGrid grid;
    EntityColliderArray dynamicColliders;
    Arena* permanentArena;
    Arena* frameArena;
};

CORE_API void importCollisionModule(Ecs* ecs);
CORE_API void initCollisionManager(Arena* arena);
CORE_API void collisionStartFrame();
CORE_API void collisionEndFrame();
CORE_API CollisionEventArray* getCollisionEvents();
CORE_API TriggerEventArray* getTriggerEvents();
CORE_API TriggerEventArray* getTriggerEnterEvents();
CORE_API void updateCollisions(Ecs* ecs);
CORE_API void systemResolvePhysicsCollisions(Ecs* ecs);

CORE_API Entity getNearestEntity(Ecs* ecs, Entity e, int cellRange);
CORE_API EntityColliderArray* getNearestEntities(Ecs* ecs, Entity entity, float radius);

CORE_API void systemUpdateColliderPosition(Ecs* ecs);
CORE_API void systemUpdateTransformChildEntities(Ecs* ecs);
CORE_API void systemRenderColliders(Ecs* ecs);
CORE_API void setGridCenter(float x, float y);
CORE_API void renderGrid();