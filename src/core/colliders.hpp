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
    //Entity sensorBeginTouch;
};

extern ECS_DECLARE_COMPONENT_EXTERN(HitBox)
struct HitBox{
    float dmg = 0;
    //Box2DCollider area;
    //Entity hittedEntity;
    //bool hit = false;
    //bool alreadyHitted = false;
    glm::vec2 offset = {0,0};
    glm::vec2 size = {0,0};
    glm::vec2 relativePosition = {0,0};
    //bool discover = false;
};

extern ECS_DECLARE_COMPONENT_EXTERN(HurtBox)
struct HurtBox{
    float health;
    bool invincible = false;
    glm::vec2 offset = {0,0};
    glm::vec2 size = {0,0};
    glm::vec2 relativePosition = {0,0};
    //Box2DCollider area;
    //Entity hittedByEntity;
    //bool hit = false;
    //bool hitted = false;
};

CORE_API void importCollisionModule(Ecs* ecs);
CORE_API void initCollisionManager();
CORE_API Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box);
CORE_API Box2DCollider calculateCollider(TransformComponent* transform, glm::vec2 offset, glm::vec2 size);
//bool isColliding(const Box2DCollider* a, const Box2DCollider* b) ;
CORE_API bool beginCollision(const Entity a, const Entity b);
CORE_API bool endCollision(const Entity a, const Entity b);
CORE_API bool isColliding(const Entity a, const Entity b);
//CORE_API bool hitCollision(const Entity a, const Entity b);
//CORE_API void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB);
//CORE_API void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB);
CORE_API void systemCheckCollisions(Ecs* ecs, Entity player);
CORE_API void clearCollisions();
//CORE_API void systemCheckCollisionDynamicStatic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
//CORE_API void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
CORE_API void systemResolvePhysicsCollisions(Ecs* ecs);
CORE_API glm::vec2 getBoxCenter(const Box2DCollider* box);
CORE_API glm::vec2 getBoxCenter(const glm::vec2* position, const glm::vec2* size);