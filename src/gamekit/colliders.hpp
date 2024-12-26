#pragma once
#include "core/ecs.hpp"
#include "export.hpp"

struct Box2DCollider{
    //TODO: phisics body instead of do in in collider???
    enum ColliderType {DYNAMIC, STATIC};
    ColliderType type;
    bool active = true;
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};

    bool onCollision = false;
};

KIT_API Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box);
KIT_API bool isColliding(const Box2DCollider* a, const Box2DCollider* b) ;
KIT_API void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB);
KIT_API void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB);
KIT_API void systemCheckCollisionDynamicStatic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);
KIT_API void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt);