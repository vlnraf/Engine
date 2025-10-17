#include "colliders.hpp"
#include "core/tilemap.hpp"
#include <unordered_set>

#define COLLIDER_LAYER 1

//NOTE: if the cell size is too low right now it happens to not detect correct collisions
// the reason is that i don't search for all the neighborhood of the collider itself but only from it's position
// so i don't check all the neighborhoods
#define CELL_SIZE_X 32
#define CELL_SIZE_Y 32

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
    result.x = box->relativePosition.x + (0.5f * box->size.x);
    result.y = box->relativePosition.y + (0.5f * box->size.y);
    return result;
}

glm::vec2 getBoxCenter(const glm::vec2* position, const glm::vec2* size){
    glm::vec2 result;
    result.x = position->x + (0.5f * size->x);
    result.y = position->y + (0.5f * size->y);
    return result;
}

std::size_t spatialHash(const glm::vec2& position){
    //NOTE: big prime numbers to reduce collision
    const size_t PRIME = 73856093;
    const size_t PRIME2 = 19349663;
    size_t x = (size_t)floor(position.x / CELL_SIZE_X);
    size_t y = (size_t)floor(position.y / CELL_SIZE_Y);
    return (x * PRIME) ^ (y * PRIME2);
}

std::unordered_map<int, std::vector<Entity>> spatialGrid;
std::unordered_map<int, std::vector<Entity>> spatialGridHitBoxes;

std::vector<Entity> getNearbyEntities(const glm::vec2& position){
    std::vector<Entity> result;
    std::unordered_set<Entity> seen;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            size_t cell = spatialHash({((position.x / CELL_SIZE_X) + i) * CELL_SIZE_X, ((position.y / CELL_SIZE_Y) + j) * CELL_SIZE_Y});
            auto it = spatialGrid.find(cell);
            if(it != spatialGrid.end()){
                for(Entity e : it->second){
                    if(seen.insert(e).second){
                        result.push_back(e);
                    }
                }
            }
        }
    }
    return result;
}

std::vector<Entity> getNearbyHitHurtboxes(const glm::vec2& position){
    std::vector<Entity> result;
    std::unordered_set<Entity> seen;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            size_t cell = spatialHash({((position.x / CELL_SIZE_X) + i) * CELL_SIZE_X, ((position.y / CELL_SIZE_Y) + j) * CELL_SIZE_Y});
            auto it = spatialGridHitBoxes.find(cell);
            if(it != spatialGridHitBoxes.end()){
                for(Entity e : it->second){
                    if(seen.insert(e).second){
                        result.push_back(e);
                    }
                }
            }
        }
    }
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
        if((collisionEvent.entityA == a && collisionEvent.entityB == b) ||
            (collisionEvent.entityB == a && collisionEvent.entityA == b)){
            check = true;
            break;
        }
    }
    return check;
}

bool searchBeginCollision(Entity a, Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : beginCollisionEvents){
        if((collisionEvent.entityA == a && collisionEvent.entityB == b) ||
            (collisionEvent.entityB == a && collisionEvent.entityA == b)){
            check = true;
            break;
        }
    }
    return check;
}
bool searchEndCollision(Entity a, Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : endCollisionEvents){
        if((collisionEvent.entityA == a && collisionEvent.entityB == b) ||
            (collisionEvent.entityB == a && collisionEvent.entityA == b)){
            check = true;
            break;
        }
    }
    return check;
}

bool isColliding(const Entity a, const Entity b){
    bool check = false;
    for(CollisionEvent collisionEvent : collisionEvents){
        if((collisionEvent.entityA == a && collisionEvent.entityB == b) ||
            (collisionEvent.entityB == a && collisionEvent.entityA == b)){
            check = true;
            break;
        }
    }
    return check;
}
//bool searchHitCollision(Entity a, Entity b){
//    bool check = false;
//    for(CollisionEvent collisionEvent : collisionEvents){
//        if(collisionEvent.entityA == a && collisionEvent.entityB == b){
//            check =  true;
//            break;
//        }
//    }
//    return check;
//}

bool beginCollision(const Entity a, const Entity b){
    return searchBeginCollision(a, b);
}

//bool hitCollision(const Entity a, const Entity b){
//    return searchHitCollision(a, b);
//}

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

    //float correctionX = overlapX * 0.5f;
    //float correctionY = overlapY * 0.5f;
    //tA->position.x -= correctionX;
    //tA->position.y -= correctionY;

    //Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        //float correction = overlapX / 2.0f;
        float correction = (overlapX / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
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
        float correction = (overlapY / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
            tB->position.y += correction;
        } else {
            tA->position.y += correction;
            tB->position.y -= correction;
        }
    }
}

void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, Box2DCollider* boxA, Box2DCollider* boxB){
    TransformComponent* tA = getComponent(ecs, entityA, TransformComponent);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    //float correctionX = overlapX * 0.5f;
    //float correctionY = overlapY * 0.5f;
    //if (boxA->offset.x < boxB->offset.x) {
    //    tA->position.x -= correctionX;
    //}else{
    //    tA->position.x += correctionX;
    //}

    //if (boxA->offset.y < boxB->offset.y) {
    //    tA->position.y -= correctionY;
    //}else{
    //    tA->position.y += correctionY;
    //}

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        //float correction = (overlapY / 2.0f);
        float correction = (overlapX / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
        } else {
            tA->position.x += correction;
        }
    } else {
        // Resolve along Y-axis
        //float correction = (overlapY / 2.0f);
        float correction = (overlapY / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
        } else {
            tA->position.y += correction;
        }
    }
}

void systemResolvePhysicsCollisions(Ecs* ecs){
    for(CollisionEvent collision : collisionEvents){
        if(collision.type == TRIGGER) continue;
        Box2DCollider* boxAent = getComponent(ecs, collision.entityA, Box2DCollider);
        TransformComponent* tA= getComponent(ecs, collision.entityA, TransformComponent);
        Box2DCollider* boxBent = getComponent(ecs, collision.entityB, Box2DCollider);
        TransformComponent* tB = getComponent(ecs, collision.entityB, TransformComponent);
        Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
        Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
        if(boxAent->type == Box2DCollider::STATIC && boxBent->type == Box2DCollider::STATIC) continue;
        if(boxAent->type == Box2DCollider::STATIC){
            resolveDynamicStaticCollision(ecs, collision.entityB, &boxB, &boxA);
        }else if(boxBent->type == Box2DCollider::STATIC){
            resolveDynamicStaticCollision(ecs, collision.entityA, &boxA, &boxB);
        }else{
            resolveDynamicDynamicCollision(ecs, collision.entityA, collision.entityB, &boxA, &boxB);
        }
    }
}

void checkCollision(Ecs* ecs, const std::vector<Entity> entities, std::vector<Entity> hitHurtBoxes){
    for(Entity entityA : entities){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, Box2DCollider);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
        Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
        std::vector<Entity> collidedEntities = getNearbyEntities(boxA.offset);
        for(Entity entityB : collidedEntities){
            if(entityA == entityB) continue;
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, Box2DCollider);
            if(boxAent->type == Box2DCollider::STATIC && boxBent->type == Box2DCollider::STATIC) continue;
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 

            bool previosFrameCollision = searchCollisionPrevFrame(entityA, entityB);
            CollisionType collisionType = (boxAent->isTrigger || boxBent->isTrigger) ? CollisionType::TRIGGER : CollisionType::PHYSICS;

            if(previosFrameCollision && isColliding(&boxA, &boxB)){
                collisionEvents.push_back({entityA, entityB, collisionType});
            }
            if(!previosFrameCollision && isColliding(&boxA, &boxB)){
                collisionEvents.push_back({entityA, entityB, collisionType});
                beginCollisionEvents.push_back({entityA, entityB, collisionType});
            }
            if(previosFrameCollision && !isColliding(&boxA, &boxB)){
                endCollisionEvents.push_back({entityA, entityB, collisionType});
            }
        }
    }

    //Hitbox and hurtboxes too
    //auto hitboxes = view(ecs, HitBox);
    //auto hurtboxes = view(ecs, HurtBox);
    for(Entity entityA : hitHurtBoxes){
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, HitBox);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
        Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
        std::vector<Entity> collidedEntities = getNearbyHitHurtboxes(boxA.offset);
        for(Entity entityB : collidedEntities){
            if(entityA == entityB) continue;
            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, HurtBox);
            if(!boxBent){ continue;}
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
            Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 
            //boxA.offset = {boxA.offset.x, boxA.offset.y}; 
            //boxB.offset = {boxB.offset.x, boxB.offset.y};
            //boxA.size = {boxA.size.x, boxA.size.y}; 
            //boxB.size = {boxB.size.x, boxB.size.y}; 

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

void systemCheckCollisions(Ecs* ecs){
    EntityArray colliderEntities = view(ecs, Box2DCollider);
    std::vector<Entity> dynamicColliders;
    std::vector<Entity> hitHurtBoxes;
    spatialGrid.clear();
    spatialGridHitBoxes.clear();
    dynamicColliders.clear();
    hitHurtBoxes.clear();

    //Insert for spatial hashing
    //for(Entity e : colliderEntities){
    for(size_t i = 0; i < colliderEntities.count; i++){
        Entity e = colliderEntities.entities[i];
        TransformComponent* t = getComponent(ecs, e, TransformComponent);
        Box2DCollider* box = getComponent(ecs, e, Box2DCollider);
        Box2DCollider bb = calculateWorldAABB(t, box);
        int minX = (bb.offset.x / CELL_SIZE_X);
        int maxX = ((bb.offset.x + bb.size.x) / CELL_SIZE_X);
        int minY = (bb.offset.y / CELL_SIZE_Y);
        int maxY = ((bb.offset.y + bb.size.y) / CELL_SIZE_Y);
        for(int x = minX; x <= maxX; x++){
            for(int y = minY; y <= maxY; y++){
                int cell = spatialHash({x * CELL_SIZE_X, y * CELL_SIZE_Y});
                spatialGrid[cell].push_back(e);
            }
        }
        //Prune static colliders to check onlyl dynamics vs static
        if(box->type == Box2DCollider::STATIC){ continue; }
        dynamicColliders.push_back(e);
    }

    EntityArray hitboxes = view(ecs, HitBox);
    EntityArray hurtboxes = view(ecs, HurtBox);
    //for(Entity e : hitboxes){
    for(size_t i = 0; i < hitboxes.count; i++ ){
        Entity e = hitboxes.entities[i];
        TransformComponent* t = getComponent(ecs, e, TransformComponent);
        HitBox* box = getComponent(ecs, e, HitBox);
        Box2DCollider bb = calculateCollider(t, box->offset, box->size);
        int minX = (bb.offset.x / CELL_SIZE_X);
        int maxX = ((bb.offset.x + bb.size.x) / CELL_SIZE_X);
        int minY = (bb.offset.y / CELL_SIZE_Y);
        int maxY = ((bb.offset.y + bb.size.y) / CELL_SIZE_Y);
        for(int x = minX; x <= maxX; x++){
            for(int y = minY; y <= maxY; y++){
                int cell = spatialHash({x * CELL_SIZE_X, y * CELL_SIZE_Y});
                spatialGridHitBoxes[cell].push_back(e);
            }
        }
        hitHurtBoxes.push_back(e);
    }

    //for(Entity e : hurtboxes){
    for(size_t i = 0; i < hurtboxes.count; i++ ){
        Entity e = hurtboxes.entities[i];
        TransformComponent* t = getComponent(ecs, e, TransformComponent);
        HurtBox* box = getComponent(ecs, e, HurtBox);
        Box2DCollider bb = calculateCollider(t, box->offset, box->size);
        int minX = (bb.offset.x / CELL_SIZE_X);
        int maxX = ((bb.offset.x + bb.size.x) / CELL_SIZE_X);
        int minY = (bb.offset.y / CELL_SIZE_Y);
        int maxY = ((bb.offset.y + bb.size.y) / CELL_SIZE_Y);
        for(int x = minX; x <= maxX; x++){
            for(int y = minY; y <= maxY; y++){
                int cell = spatialHash({x * CELL_SIZE_X, y * CELL_SIZE_Y});
                spatialGridHitBoxes[cell].push_back(e);
            }
        }
    }
    collisionEventsPrevFrame = collisionEvents;
    collisionEvents.clear();
    beginCollisionEvents.clear();
    endCollisionEvents.clear();
    checkCollision(ecs, dynamicColliders, hitHurtBoxes);
}