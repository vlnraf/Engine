#include "colliders.hpp"

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

void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB){
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, ECS_TRANSFORM);
    TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB, ECS_TRANSFORM);

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
}

void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB){
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, ECS_TRANSFORM);
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
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, ECS_2D_BOX_COLLIDER);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        //VelocityComponent* velA = (VelocityComponent*) getComponent(ecs, entityA, ECS_VELOCITY);
        //DirectionComponent* dirA = (DirectionComponent*) getComponent(ecs, entityA, ECS_DIRECTION);
        for(Entity entityB : entitiesB){
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, ECS_2D_BOX_COLLIDER);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            //boxA.offset = {boxA.offset.x + (velA->vel.x * dirA->dir.x * dt), boxA.offset.y + (velA->vel.y * dirA->dir.y * dt)}; 
            boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            //boxB.offset = {boxB.offset.x + (velB->vel.x * dirB->dir.x * dt), boxB.offset.y + (velB->vel.y * dirB->dir.y * dt)}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y};
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            if(boxAent->active && boxBent->active){
                if(isColliding(&boxA, &boxB)){
                    boxAent->onCollision = true;
                    boxBent->onCollision = true;
                    resolveDynamicStaticCollision(ecs, entityA, entityB, &boxA, &boxB);
                }else{
                    boxAent->onCollision = false;
                    boxBent->onCollision = false;
                }
            }
        }
    }
}

void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, ECS_2D_BOX_COLLIDER);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        //VelocityComponent* velA = (VelocityComponent*) getComponent(ecs, entityA, ECS_VELOCITY);
        //DirectionComponent* dirA = (DirectionComponent*) getComponent(ecs, entityA, ECS_DIRECTION);
        for(Entity entityB : entitiesB){
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, ECS_2D_BOX_COLLIDER);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            //NOTE: check the collision on the next frame only for dynamic colliders
            //VelocityComponent* velB = (VelocityComponent*) getComponent(ecs, entityB, ECS_VELOCITY);
            //DirectionComponent* dirB = (DirectionComponent*) getComponent(ecs, entityB, ECS_DIRECTION);
            //boxA.offset = {boxA.offset.x + (velA->vel.x * dirA->dir.x * dt), boxA.offset.y + (velA->vel.y * dirA->dir.y * dt)}; 
            //boxB.offset = {boxB.offset.x + (velB->vel.x * dirB->dir.x * dt), boxB.offset.y + (velB->vel.y * dirB->dir.y * dt)}; 
            boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y}; 
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            if(boxAent->active && boxBent->active){
                if(isColliding(&boxA, &boxB)){
                    boxAent->onCollision = true;
                    boxBent->onCollision = true;
                    resolveDynamicDynamicCollision(ecs, entityA, entityB, &boxA, &boxB);
                }else{
                    boxAent->onCollision = false;
                    boxBent->onCollision = false;
                }
            }
        }
    }
}