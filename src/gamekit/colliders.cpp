#include "colliders.hpp"
#include "projectile.hpp"

Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box){
    Box2DCollider newBox;
    newBox.offset.x = transform->position.x + box->offset.x;
    newBox.offset.y = transform->position.y + box->offset.y;
    newBox.size = box->size;

    //NOTE: should i duplicate???, think a better approach if needed
    newBox.active = box->active;
    newBox.type = box->type;
    newBox.onCollision = box->onCollision;
    return newBox;
}

glm::vec2 getBoxCenter(const Box2DCollider* box){
    glm::vec2 result;
    result.x = box->offset.x + (0.5 * box->size.x);
    result.y = box->offset.y + (0.5 * box->size.y);
    return result;
}

bool isColliding(const Box2DCollider* a, const Box2DCollider* b) {
      return (a->offset.x < b->offset.x + b->size.x &&
              a->offset.x + a->size.x > b->offset.x &&
              a->offset.y < b->offset.y + b->size.y &&
              a->offset.y + a->size.y > b->offset.y);
}

void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB){
    TransformComponent* tA = getComponent(ecs, entityA, TransformComponent);
    TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);
    //TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, ECS_TRANSFORM);
    //TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB, ECS_TRANSFORM);

    //VelocityComponent* velA = (VelocityComponent*)getComponent(ecs, entityA, ECS_VELOCITY);
    //VelocityComponent* velB = (VelocityComponent*)getComponent(ecs, entityB, ECS_VELOCITY);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        float correction = overlapX / 2.0f;
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
            tB->position.x += correction;
        } else {
            tA->position.x += correction;
            tB->position.x -= correction;
        }
    } else {
        // Resolve along Y-axis
        float correction = overlapY / 2.0f;
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
            tB->position.y += correction;
        } else {
            tA->position.y += correction;
            tB->position.y -= correction;
        }
    }
    boxA->onCollision = false;
    boxB->onCollision = false;
}

void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB){
    TransformComponent* tA = getComponent(ecs, entityA, TransformComponent);
    //TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, ECS_TRANSFORM);
    //TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB, ECS_TRANSFORM);

    //VelocityComponent* velA = (VelocityComponent*)getComponent(ecs, entityA, ECS_VELOCITY);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        float correction = overlapX / 2.0f;
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
        } else {
            tA->position.x += correction;
        }
    } else {
        // Resolve along Y-axis
        float correction = overlapY / 2.0f;
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
        } else {
            tA->position.y += correction;
        }
    }
}

void systemCheckCollisionDynamicStatic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, Box2DCollider);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
        for(Entity entityB : entitiesB){
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, Box2DCollider);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y};
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            if(isColliding(&boxA, &boxB)){
                if(boxAent->active && boxBent->active){
                    boxAent->onCollision = true;
                    boxBent->onCollision = true;
                    if(hasComponent(ecs, entityA, ProjectileTag)){
                        destroyProjectile(ecs, entityA);
                        break;
                    }else{
                        resolveDynamicStaticCollision(ecs, entityA, entityB, &boxA, &boxB);
                    }
                }
            }
        }
    }
}

void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, Box2DCollider);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
        //VelocityComponent* velA = (VelocityComponent*) getComponent(ecs, entityA, ECS_VELOCITY);
        //DirectionComponent* dirA = (DirectionComponent*) getComponent(ecs, entityA, ECS_DIRECTION);
        for(Entity entityB : entitiesB){
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, Box2DCollider);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            //NOTE: check the collision on the next frame only for dynamic colliders
            //VelocityComponent* velB = (VelocityComponent*) getComponent(ecs, entityB, ECS_VELOCITY);
            //DirectionComponent* dirB = (DirectionComponent*) getComponent(ecs, entityB, ECS_DIRECTION);
            boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y}; 
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            if(isColliding(&boxA, &boxB)){
                if(boxAent->active && boxBent->active){
                    boxAent->onCollision = true;
                    boxBent->onCollision = true;
                    resolveDynamicDynamicCollision(ecs, entityA, entityB, &boxA, &boxB);
                }
            }
        }
    }
}