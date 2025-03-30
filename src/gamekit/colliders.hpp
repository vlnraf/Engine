#pragma once
#include "core/ecs.hpp"
//#include "export.hpp"

struct Box2DCollider{
    //TODO: phisics body instead of do in in collider???
    enum ColliderType {DYNAMIC, STATIC};
    ColliderType type;
    //bool active = true;
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};
    bool isTrigger = false;
    //Entity sensorBeginTouch;
};

struct HitBox{
    int dmg = 0;
    //Box2DCollider area;
    //Entity hittedEntity;
    //bool hit = false;
    //bool alreadyHitted = false;
    glm::vec2 offset;
    glm::vec2 size;
    //bool discover = false;
};

struct HurtBox{
    int health;
    bool invincible = false;
    glm::vec2 offset;
    glm::vec2 size;
    //Box2DCollider area;
    //Entity hittedByEntity;
    //bool hit = false;
    //bool hitted = false;
};

Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box);
Box2DCollider calculateCollider(TransformComponent* transform, glm::vec2 offset, glm::vec2 size);
//bool isColliding(const Box2DCollider* a, const Box2DCollider* b) ;
bool beginCollision(const Entity a, const Entity b);
bool hitCollision(const Entity a, const Entity b);
bool endCollision(const Entity a, const Entity b);
void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB);
void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB);
void systemCheckCollisionDynamicStatic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
void systemCheckCollisions(Ecs* ecs, float dt);
void systemResolvePhysicsCollisions(Ecs* ecs, const float dt);
glm::vec2 getBoxCenter(const Box2DCollider* box);