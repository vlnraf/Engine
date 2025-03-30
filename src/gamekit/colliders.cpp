#include "colliders.hpp"
#include "projectile.hpp"

enum CollisionType{
    PHYSICS,
    TRIGGER
};

struct CollisionEvent{
    Entity entityA;
    Entity entityB;
    CollisionType type;
};

std::vector<CollisionEvent> beginCollisionEvents;
std::vector<CollisionEvent> endCollisionEvents;
//std::vector<CollisionEvent> hitCollisionEvents;
std::vector<CollisionEvent> collisionEvents;
std::vector<CollisionEvent> collisionEventsPrevFrame;

Box2DCollider calculateCollider(TransformComponent* transform, glm::vec2 offset, glm::vec2 size){
    Box2DCollider newBox;
    newBox.offset.x = transform->position.x + offset.x;
    newBox.offset.y = transform->position.y + offset.y;
    newBox.size = size;
    return newBox;
}

Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box){
    Box2DCollider newBox;
    newBox.offset.x = transform->position.x + box->offset.x;
    newBox.offset.y = transform->position.y + box->offset.y;
    newBox.size = box->size;
    newBox.type = box->type;
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

bool searchCollisionPrevFrame(Entity a, Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : collisionEventsPrevFrame){
        if(collisionEvent.entityA == a && collisionEvent.entityB == b){
            check = true;
            break;
        }
    }
    return check;
}

bool searchBeginCollision(Entity a, Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : beginCollisionEvents){
        if(collisionEvent.entityA == a && collisionEvent.entityB == b){
            check = true;
            break;
        }
    }
    return check;
}
bool searchEndCollision(Entity a, Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : endCollisionEvents){
        if(collisionEvent.entityA == a && collisionEvent.entityB == b){
            check = true;
            break;
        }
    }
    return check;
}
bool searchHitCollision(Entity a, Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : collisionEvents){
        if(collisionEvent.entityA == a && collisionEvent.entityB == b){
            check =  true;
            break;
        }
    }
    return check;
}

bool beginCollision(const Entity a, const Entity b){
    return searchBeginCollision(a, b);
}

bool hitCollision(const Entity a, const Entity b){
    return searchHitCollision(a, b);
}

bool endCollision(const Entity a, const Entity b){
    return searchEndCollision(a, b);
}

void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB){
    TransformComponent* tA = getComponent(ecs, entityA, TransformComponent);
    TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        //float correction = overlapX / 2.0f;
        float correction = (overlapX / 2.0f) + 0.1; //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
            tB->position.x += correction;
        } else {
            tA->position.x += correction;
            tB->position.x -= correction;
        }
    } else {
        // Resolve along Y-axis
        //float correction = overlapY / 2.0f;
        float correction = (overlapY / 2.0f) + 0.001; //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
            tB->position.y += correction;
        } else {
            tA->position.y += correction;
            tB->position.y -= correction;
        }
    }
}

void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, Box2DCollider* boxA, Box2DCollider* boxB){
    TransformComponent* tA = getComponent(ecs, entityA, TransformComponent);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        //float correction = (overlapY / 2.0f);
        float correction = (overlapX / 2.0f) + 0.1; //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
        } else {
            tA->position.x += correction;
        }
    } else {
        // Resolve along Y-axis
        //float correction = (overlapY / 2.0f);
        float correction = (overlapY / 2.0f) + 0.001; //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
        } else {
            tA->position.y += correction;
        }
    }
}

void systemResolvePhysicsCollisions(Ecs* ecs, const float dt){
    for(CollisionEvent collision : collisionEvents){
        if(collision.type == TRIGGER) continue;
        Box2DCollider* boxAent = getComponent(ecs, collision.entityA, Box2DCollider);
        TransformComponent* tA= getComponent(ecs, collision.entityA, TransformComponent);
        Box2DCollider* boxBent = getComponent(ecs, collision.entityB, Box2DCollider);
        TransformComponent* tB = getComponent(ecs, collision.entityB, TransformComponent);
        Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
        Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
        boxA.offset = {boxA.offset.x, boxA.offset.y}; 
        boxB.offset = {boxB.offset.x, boxB.offset.y};
        boxA.size = {boxA.size.x, boxA.size.y}; 
        boxB.size = {boxB.size.x, boxB.size.y}; 
        if(boxAent->type == Box2DCollider::STATIC && boxBent->type == Box2DCollider::STATIC) continue;
        if(boxAent->type == Box2DCollider::STATIC){
            resolveDynamicStaticCollision(ecs, collision.entityB, collision.entityA, &boxA, & boxB);
        }else if(boxBent->type == Box2DCollider::STATIC){
            resolveDynamicStaticCollision(ecs, collision.entityA, collision.entityB, &boxA, & boxB);
        }else{
            resolveDynamicDynamicCollision(ecs, collision.entityA, collision.entityB, &boxA, &boxB);
        }
    }
}

void systemCheckCollision(Ecs* ecs, const std::vector<Entity> entities, const float dt){
    for(Entity entityA : entities){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, Box2DCollider);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
        for(Entity entityB : entities){
            if(entityA == entityB) continue;
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, Box2DCollider);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y};
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            bool previosFrameCollision = searchCollisionPrevFrame(entityA, entityB);
            CollisionType collisionType = (boxAent->isTrigger || boxBent->isTrigger) ? CollisionType::TRIGGER : CollisionType::PHYSICS;

            if(previosFrameCollision && isColliding(&boxA, &boxB)){
                collisionEvents.push_back({entityA, entityB, collisionType});
            }
            if(!previosFrameCollision && isColliding(&boxA, &boxB)){
                collisionEvents.push_back({entityA, entityB, collisionType});
                beginCollisionEvents.push_back({entityA, entityB});
            }
            if(previosFrameCollision && !isColliding(&boxA, &boxB)){
                endCollisionEvents.push_back({entityA, entityB, collisionType});
            }
        }
    }

    //Hitbox and hurtboxes too
    auto hitboxes = view(ecs, HitBox);
    auto hurtboxes = view(ecs, HurtBox);
    for(Entity entityA : hitboxes){
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, HitBox);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
        for(Entity entityB : hurtboxes){
            if(entityA == entityB) continue;
            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, HurtBox);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
            Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 
            boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y};
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            bool previosFrameCollision = searchCollisionPrevFrame(entityA, entityB);

            if(previosFrameCollision && isColliding(&boxA, &boxB)){
                collisionEvents.push_back({entityA, entityB, TRIGGER});
            }
            if(!previosFrameCollision && isColliding(&boxA, &boxB)){
                collisionEvents.push_back({entityA, entityB, TRIGGER});
                beginCollisionEvents.push_back({entityA, entityB, TRIGGER});
            }
            if(previosFrameCollision && !isColliding(&boxA, &boxB)){
                endCollisionEvents.push_back({entityA, entityB, TRIGGER});
            }
        }
    }
}

void systemCheckCollisions(Ecs* ecs, float dt){
    std::vector<Entity> dynamicEntities;
    std::vector<Entity> staticEntities;
    std::vector<Entity> colliderEntities = view(ecs, Box2DCollider);
    collisionEventsPrevFrame = collisionEvents;
    collisionEvents.clear();
    beginCollisionEvents.clear();
    endCollisionEvents.clear();
    systemCheckCollision(ecs, colliderEntities, dt);
}