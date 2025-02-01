#pragma once
#include "core/ecs.hpp"
//#include "export.hpp"

struct Box2DCollider{
    //TODO: phisics body instead of do in in collider???
    enum ColliderType {DYNAMIC, STATIC};
    ColliderType type;
    bool active = true;
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};

    bool onCollision = false;
};

Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box);
bool isColliding(const Box2DCollider* a, const Box2DCollider* b) ;
void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB);
void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB);
void systemCheckCollisionDynamicStatic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
glm::vec2 getBoxCenter(const Box2DCollider* box);