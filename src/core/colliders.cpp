#include "colliders.hpp"
#include "core/tilemap.hpp"
#include "componentIds.hpp"
#include "ecs.hpp"
#include "spike.hpp"

#include <unordered_set>

#if 1

#define COLLIDER_LAYER 1

//NOTE: if the cell size is too low right now it happens to not detect correct collisions
// the reason is that i don't search for all the neighborhood of the collider itself but only from it's position
// so i don't check all the neighborhoods
#define CELL_SIZE_X 32
#define CELL_SIZE_Y 32
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
#define MAX_CELLS GRID_WIDTH * GRID_HEIGHT
#define MAX_CELL_ENTITIES 30

enum CollisionType{
    PHYSICS,
    TRIGGER
};

struct EntityCollider{
    Entity entity;
    union{
        Box2DCollider* collider;
        HitBox* hitbox;
        HurtBox* hurtbox;
    };
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

struct EntityColliderArray{
    EntityCollider* item;
    size_t count = 0;
};

struct CollisionGrid{
    float originX, originY = 0;
    float centerX, centerY = 0;

    EntityColliderArray* cell;
};


CollisionEventArray* beginCollisionEvents;
CollisionEventArray* endCollisionEvents;
CollisionEventArray* collisionEvents;
CollisionEventArray* collisionEventsPrevFrame;
CollisionGrid* grid;
Arena* permanentArena;
Arena* frameArena;

void initCollisionManager(){
    permanentArena = initArena(MB(64));
    frameArena = initArena(MB(64));
    grid = arenaAllocStructZero(permanentArena, CollisionGrid);
    collisionEventsPrevFrame = arenaAllocStructZero(permanentArena, CollisionEventArray);
    collisionEventsPrevFrame->item = arenaAllocArrayZero(permanentArena, CollisionEvent, MAX_ENTITIES);
}

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

bool isColliding(const Box2DCollider* a, const Box2DCollider* b) {
    return (a->relativePosition.x < b->relativePosition.x + b->size.x &&
            a->relativePosition.x + a->size.x > b->relativePosition.x &&
            a->relativePosition.y < b->relativePosition.y + b->size.y &&
            a->relativePosition.y + a->size.y > b->relativePosition.y);
}

bool isColliding(const Entity a, const Entity b){
    bool check = false;
    for(int i = 0; i < collisionEvents->count; i++){
        CollisionEvent collisionEvent = collisionEvents->item[i];
        if((collisionEvent.entityA.entity == a && collisionEvent.entityB.entity == b) ||
            (collisionEvent.entityB.entity == a && collisionEvent.entityA.entity == b)){
            check = true;
            break;
        }
    }
    return check;
}

bool searchCollisionPrevFrame(Entity a, Entity b){
    bool check = false;
    for(int i = 0; i < collisionEventsPrevFrame->count; i++){
        CollisionEvent collisionEvent = collisionEventsPrevFrame->item[i];
        if((collisionEvent.entityA.entity == a && collisionEvent.entityB.entity == b) ||
            (collisionEvent.entityB.entity == a && collisionEvent.entityA.entity == b)){
            check = true;
            break;
        }
    }
    return check;
}

bool searchBeginCollision(Entity a, Entity b){
    bool check = false;
    for(int i = 0; i < beginCollisionEvents->count; i++){
        CollisionEvent collisionEvent = beginCollisionEvents->item[i];
        if((collisionEvent.entityA.entity == a && collisionEvent.entityB.entity == b) ||
            (collisionEvent.entityB.entity == a && collisionEvent.entityA.entity == b)){
            check = true;
            break;
        }
    }
    return check;
}
bool searchEndCollision(Entity a, Entity b){
    bool check = false;
    for(int i = 0; i < endCollisionEvents->count; i++){
        CollisionEvent collisionEvent = endCollisionEvents->item[i];
        if((collisionEvent.entityA.entity == a && collisionEvent.entityB.entity == b) ||
            (collisionEvent.entityB.entity == a && collisionEvent.entityA.entity == b)){
            check = true;
            break;
        }
    }
    return check;
}

bool beginCollision(const Entity a, const Entity b){
    return searchBeginCollision(a, b);
}

bool endCollision(const Entity a, const Entity b){
    return searchEndCollision(a, b);
}

void resolveDynamicDynamicCollision(Ecs* ecs, const EntityCollider entityA, const EntityCollider entityB){
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA.entity, transformComponentId);
    TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB.entity, transformComponentId);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(entityA.collider->relativePosition.x + entityA.collider->size.x, entityB.collider->relativePosition.x + entityB.collider->size.x) -
                     std::max(entityA.collider->relativePosition.x, entityB.collider->relativePosition.x);
    float overlapY = std::min(entityA.collider->relativePosition.y + entityA.collider->size.y, entityB.collider->relativePosition.y + entityB.collider->size.y) -
                     std::max(entityA.collider->relativePosition.y, entityB.collider->relativePosition.y);


    //Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        //float correction = overlapX / 2.0f;
        float correction = (overlapX / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (entityA.collider->relativePosition.x < entityB.collider->relativePosition.x) {
            tA->position.x -= correction;
            tB->position.x += correction;
            entityA.collider->relativePosition.x -= correction;
            entityB.collider->relativePosition.x += correction;
        } else {
            tA->position.x += correction;
            tB->position.x -= correction;
            entityA.collider->relativePosition.x += correction;
            entityB.collider->relativePosition.x -= correction;
        }
    } else {
        // Resolve along Y-axis
        //float correction = overlapY / 2.0f;
        float correction = (overlapY / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (entityA.collider->relativePosition.y < entityB.collider->relativePosition.y) {
            tA->position.y -= correction;
            tB->position.y += correction;
            entityA.collider->relativePosition.y -= correction;
            entityB.collider->relativePosition.y += correction;
        } else {
            tA->position.y += correction;
            tB->position.y -= correction;
            entityA.collider->relativePosition.y += correction;
            entityB.collider->relativePosition.y -= correction;
        }
    }
}

void resolveDynamicStaticCollision(Ecs* ecs, const EntityCollider entityA, const EntityCollider entityB){
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA.entity, transformComponentId);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(entityA.collider->relativePosition.x + entityA.collider->size.x, entityB.collider->relativePosition.x + entityB.collider->size.x) -
                     std::max(entityA.collider->relativePosition.x, entityB.collider->relativePosition.x);
    float overlapY = std::min(entityA.collider->relativePosition.y + entityA.collider->size.y, entityB.collider->relativePosition.y + entityB.collider->size.y) -
                     std::max(entityA.collider->relativePosition.y, entityB.collider->relativePosition.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        //float correction = (overlapY / 2.0f);
        float correction = (overlapX / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (entityA.collider->relativePosition.x < entityB.collider->relativePosition.x) {
            entityA.collider->relativePosition.x -= correction;
            tA->position.x -= correction;
        } else {
            tA->position.x += correction;
            entityA.collider->relativePosition.x += correction;
        }
    } else {
        // Resolve along Y-axis
        //float correction = (overlapY / 2.0f);
        float correction = (overlapY / 2.0f); //NOTE: 0.1 to detach the objects, it's wrong, we should change method
        if (entityA.collider->relativePosition.y < entityB.collider->relativePosition.y) {
            entityA.collider->relativePosition.y -= correction;
            tA->position.y -= correction;
        } else {
            entityA.collider->relativePosition.y += correction;
            tA->position.y += correction;
        }
    }
}

void systemResolvePhysicsCollisions(Ecs* ecs){
    //LOGINFO("To implement");
    for(int i = 0; i < collisionEvents->count; i++){
        CollisionEvent collision = collisionEvents->item[i];
        if(collision.type == TRIGGER) continue;
        if(collision.entityA.collider->type == Box2DCollider::STATIC && collision.entityB.collider->type == Box2DCollider::STATIC) continue;
        if(collision.entityA.collider->type == Box2DCollider::STATIC){
            resolveDynamicStaticCollision(ecs, collision.entityB, collision.entityA);
        }else if(collision.entityB.collider->type == Box2DCollider::STATIC){
            resolveDynamicStaticCollision(ecs, collision.entityA, collision.entityB);
        }else{
            resolveDynamicDynamicCollision(ecs, collision.entityA, collision.entityB);
        }
    }
}

void checkCollision(Ecs* ecs, EntityColliderArray* colliderEntities){
    //LOGINFO("To implement");
    for(size_t i = 0; i < colliderEntities->count; i++){
        EntityCollider e = colliderEntities->item[i];
        int cellX = floorf(e.collider->relativePosition.x / CELL_SIZE_X);
        int cellY = floorf(e.collider->relativePosition.y / CELL_SIZE_Y);
        int localX = cellX - grid->originX;
        int localY = cellY - grid->originY;
        if(localX < 0 || localX >= GRID_WIDTH || localY < 0 || localY >= GRID_HEIGHT) continue;
        int minX = localX - 1;
        int maxX = localX + 1;
        int minY = localY - 1;
        int maxY = localY + 1;
        for(size_t y = minY; y <= maxY; y++){
            for(size_t x = minX; x <= maxX; x++){
                if(x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) continue;
                int index = (y * GRID_WIDTH) + x;
                EntityColliderArray nearestColliders = grid->cell[index];
                for(int j = 0; j < nearestColliders.count; j++){
                    if(e.collider->type == Box2DCollider::STATIC && nearestColliders.item[j].collider->type == Box2DCollider::STATIC) continue;
                    if(e.entity == nearestColliders.item[j].entity) continue;
                    CollisionType collisionType = (e.collider->isTrigger || nearestColliders.item[j].collider->isTrigger) ? CollisionType::TRIGGER : CollisionType::PHYSICS;
                    bool previosFrameCollision = searchCollisionPrevFrame(e.entity, nearestColliders.item[j].entity);
                    if(previosFrameCollision && isColliding(e.collider, nearestColliders.item[j].collider)){
                        CollisionEvent event = {.entityA = e, .entityB = nearestColliders.item[j], .type = collisionType};
                        collisionEvents->item[collisionEvents->count] = event;
                        collisionEvents->count++;
                    }
                    if(!previosFrameCollision && isColliding(e.collider, nearestColliders.item[j].collider)){
                        CollisionEvent event = {.entityA = e, .entityB = nearestColliders.item[j], .type = collisionType};
                        collisionEvents->item[collisionEvents->count] = event;
                        collisionEvents->count++;
                        beginCollisionEvents->item[beginCollisionEvents->count] = event;
                        beginCollisionEvents->count++;
                    }
                    if(previosFrameCollision && !isColliding(e.collider, nearestColliders.item[j].collider)){
                        CollisionEvent event = {.entityA = e, .entityB = nearestColliders.item[j], .type = collisionType};
                        endCollisionEvents->item[endCollisionEvents->count] = event;
                        endCollisionEvents->count++;
                    }
                }
            }
        }
    }
    //for(Entity entityA : hitHurtBoxes){
    //    HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, HitBox);
    //    TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, TransformComponent);
    //    Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
    //    std::vector<Entity> collidedEntities = getNearbyHitHurtboxes(boxA.offset);
    //    for(Entity entityB : collidedEntities){
    //        if(entityA == entityB) continue;
    //        HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, HurtBox);
    //        if(!boxBent){ continue;}
    //        TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, TransformComponent);
    //        //I need the position of the box which is dictated by the entity position + the box offset
    //        Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
    //        Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 

    //        bool previosFrameCollision = searchCollisionPrevFrame(entityA, entityB);

    //        if(previosFrameCollision && isColliding(&boxA, &boxB)){
    //            collisionEvents.push_back({entityA, entityB, TRIGGER});
    //        }
    //        if(!previosFrameCollision && isColliding(&boxA, &boxB)){
    //            collisionEvents.push_back({entityA, entityB, TRIGGER});
    //            beginCollisionEvents.push_back({entityA, entityB, TRIGGER});
    //        }
    //        if(previosFrameCollision && !isColliding(&boxA, &boxB)){
    //            endCollisionEvents.push_back({entityA, entityB, TRIGGER});
    //        }
    //    }
    //}
}
void checkCollisionsPerCell(Ecs* ecs) {
    for (int cellIndex = 0; cellIndex < GRID_WIDTH * GRID_HEIGHT; cellIndex++) {
        EntityColliderArray* cell = &grid->cell[cellIndex];
        for (size_t i = 0; i < cell->count; i++) {
            for (size_t j = i + 1; j < cell->count; j++) {
                EntityCollider a = cell->item[i];
                EntityCollider b = cell->item[j];

                // Skip static-static
                if (a.collider->type == Box2DCollider::STATIC && b.collider->type == Box2DCollider::STATIC)
                    continue;

                CollisionType type = (a.collider->isTrigger || b.collider->isTrigger)
                    ? TRIGGER : PHYSICS;

                bool wasColliding = searchCollisionPrevFrame(a.entity, b.entity);
                bool nowColliding = isColliding(a.collider, b.collider);

                if (wasColliding && nowColliding) {
                    //addCollisionEvent(collisionEvents, a, b, type);
                    CollisionEvent event = {.entityA = a, .entityB = b, .type = type};
                    collisionEvents->item[collisionEvents->count] = event;
                    collisionEvents->count++;
                } else if (!wasColliding && nowColliding) {
                    //addCollisionEvent(collisionEvents, a, b, type);
                    //addCollisionEvent(beginCollisionEvents, a, b, type);
                    CollisionEvent event = {.entityA = a, .entityB = b, .type = type};
                    collisionEvents->item[collisionEvents->count] = event;
                    collisionEvents->count++;
                    beginCollisionEvents->item[beginCollisionEvents->count] = event;
                    beginCollisionEvents->count++;
                } else if (wasColliding && !nowColliding) {
                    //addCollisionEvent(endCollisionEvents, a, b, type);
                    CollisionEvent event = {.entityA = a, .entityB = b, .type = type};
                    endCollisionEvents->item[endCollisionEvents->count] = event;
                    endCollisionEvents->count++;
                }
            }
        }
    }
}

void systemCheckCollisions(Ecs* ecs){
    ECS_GET_COMPONENT(ecs, 0, TEST);
    grid->cell = arenaAllocArrayZero(frameArena, EntityColliderArray, GRID_WIDTH * GRID_HEIGHT);
    for(int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++){
        grid->cell[i].item = arenaAllocStructZero(frameArena, EntityCollider);
    }

    //beginCollisionEvents->item = arenaAllocArrayZero(frameArena, CollisionEvent, MAX_ENTITIES);
    //endCollisionEvents->item = arenaAllocArrayZero(frameArena, CollisionEvent, MAX_ENTITIES);
    //collisionEvents->item = arenaAllocArrayZero(frameArena, CollisionEvent, MAX_ENTITIES);
    //LOGINFO("To implement");
    EntityArray players = view(ecs, (size_t[]){playerTagId}, 1);
    Box2DCollider* playerBox = (Box2DCollider*)getComponent(ecs, players.entities[0], box2DColliderId); //retrieve the player collider
    grid->centerX = floorf(playerBox->relativePosition.x / CELL_SIZE_X);
    grid->centerY = floorf(playerBox->relativePosition.y / CELL_SIZE_Y);
    grid->originX = grid->centerX - (GRID_WIDTH / 2);
    grid->originY = grid->centerY - (GRID_HEIGHT / 2);

    EntityColliderArray* dynamicColliders = arenaAllocStructZero(permanentArena, EntityColliderArray);
    dynamicColliders->item = arenaAllocArrayZero(frameArena, EntityCollider, MAX_ENTITIES);
    EntityArray colliderEntities = view(ecs, (size_t[]){box2DColliderId}, 1);
    for(size_t i = 0; i < colliderEntities.count; i++){
        Entity e = colliderEntities.entities[i];
        Box2DCollider* entityBox = (Box2DCollider*)getComponent(ecs, e, box2DColliderId);
        int cellX = floorf(entityBox->relativePosition.x / CELL_SIZE_X);
        int cellY = floorf(entityBox->relativePosition.y / CELL_SIZE_Y);
        int localX = cellX - grid->originX;
        int localY = cellY - grid->originY;
        if(localX < 0 || localX >= GRID_WIDTH || localY < 0 || localY >= GRID_HEIGHT) continue;
        int cellIndex = (localY * GRID_WIDTH) + localX;
        EntityCollider collider = {};
        collider.entity = e;
        collider.collider = entityBox;
        if(grid->cell[cellIndex].count >= MAX_CELL_ENTITIES){
            LOGERROR("CELL EXCEED LIMIT");
            continue;
        }
        grid->cell[cellIndex].item[grid->cell[cellIndex].count] = collider;
        grid->cell[cellIndex].count++;
        dynamicColliders->item[dynamicColliders->count] = collider;
        dynamicColliders->count++;
    }

    //for(size_t y = 0; y < GRID_HEIGHT; y++){
    //    for(size_t x = 0; x < GRID_WIDTH; x++){
    //        int cellIndex = (y * GRID_WIDTH) + x;
    //        for(size_t i = 0; i < grid->cell[cellIndex].count; i++){
    //            EntityCollider e = grid->cell[cellIndex].item[i];
    //            LOGINFO("cell: %d | num_entities: %d | entity: %d", cellIndex, grid->cell[cellIndex].count, e.entity);
    //        }
    //    }
    //}
    //dynamicColliders->item = arenaAllocArrayZero(frameArena, EntityCollider, colliderEntities.count);

    collisionEvents = arenaAllocStructZero(frameArena, CollisionEventArray);
    collisionEvents->item = arenaAllocArrayZero(frameArena, CollisionEvent, MAX_ENTITIES);
    beginCollisionEvents = arenaAllocStructZero(frameArena, CollisionEventArray);
    endCollisionEvents = arenaAllocStructZero(frameArena, CollisionEventArray);
    beginCollisionEvents->item = arenaAllocArrayZero(frameArena, CollisionEvent, MAX_ENTITIES);
    endCollisionEvents->item = arenaAllocArrayZero(frameArena, CollisionEvent, MAX_ENTITIES);
    EntityArray hitboxes = view(ecs, (size_t[]){hitBoxId}, 1);
    EntityArray hurtboxes = view(ecs, (size_t[]){hurtBoxId}, 1);
    //EntityColliderArray* hitHurtBoxes = arenaAllocStructZero(permanentArena, EntityColliderArray);
    //hitHurtBoxes->item = arenaAllocArrayZero(frameArena, EntityCollider, hitboxes.count + hurtboxes.count);
    //checkCollision(ecs, dynamicColliders);
    checkCollisionsPerCell(ecs);
    for (int i = 0; i < 6; ++i) { // tweak iterations
        systemResolvePhysicsCollisions(ecs);
    }
    //systemResolvePhysicsCollisions(ecs);
    memcpy(collisionEventsPrevFrame->item, collisionEvents->item, collisionEvents->count * sizeof(CollisionEvent));
    collisionEventsPrevFrame->count = collisionEvents->count;
    clearArena(frameArena);
    //memset(collisionEventsPrevFrame->item, 0, collisionEvents->count * sizeof(CollisionEvent));
    dynamicColliders->count = 0;
}

#else

#define CELL_SIZE_X 32
#define CELL_SIZE_Y 32
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
#define MAX_CELLS GRID_WIDTH * GRID_HEIGHT
#define MAX_CELL_ENTITIES 30

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
std::vector<CollisionEvent> collisionEvents;
std::vector<CollisionEvent> collisionEventsPrevFrame;
std::unordered_map<int, std::vector<Entity>> spatialGrid;
std::unordered_map<int, std::vector<Entity>> spatialGridHitBoxes;

void initCollisionManager(){
}
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
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, transformComponentId);
    TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB, transformComponentId);

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
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, transformComponentId);

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
        Box2DCollider* boxAent = (Box2DCollider*)getComponent(ecs, collision.entityA, box2DColliderId);
        TransformComponent* tA= (TransformComponent*)getComponent(ecs, collision.entityA, transformComponentId);
        Box2DCollider* boxBent = (Box2DCollider*)getComponent(ecs, collision.entityB, box2DColliderId);
        TransformComponent* tB = (TransformComponent*)getComponent(ecs, collision.entityB, transformComponentId);
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
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, box2DColliderId);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, transformComponentId);
        Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
        std::vector<Entity> collidedEntities = getNearbyEntities(boxA.offset);
        for(Entity entityB : collidedEntities){
            if(entityA == entityB) continue;
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, box2DColliderId);
            if(boxAent->type == Box2DCollider::STATIC && boxBent->type == Box2DCollider::STATIC) continue;
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, transformComponentId);
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
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, hitBoxId);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, transformComponentId);
        Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
        std::vector<Entity> collidedEntities = getNearbyHitHurtboxes(boxA.offset);
        for(Entity entityB : collidedEntities){
            if(entityA == entityB) continue;
            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, hurtBoxId);
            if(!boxBent){ continue;}
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, transformComponentId);
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
    EntityArray colliderEntities = view(ecs, (size_t[]){box2DColliderId}, 1);
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
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, transformComponentId);
        Box2DCollider* box = (Box2DCollider*)getComponent(ecs, e, box2DColliderId);
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

    EntityArray hitboxes = view(ecs, (size_t[]){hitBoxId}, 1);
    EntityArray hurtboxes = view(ecs, (size_t[]){hurtBoxId}, 1);
    //hitHurtBoxes.item = arenaAllocArrayZero(frameArena, EntityCollider, hitboxes.count + hurtboxes.count);
    //for(Entity e : hitboxes){
    for(size_t i = 0; i < hitboxes.count; i++ ){
        Entity e = hitboxes.entities[i];
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, transformComponentId);
        HitBox* box = (HitBox*)getComponent(ecs, e, hitBoxId);
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
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, transformComponentId);
        HurtBox* box = (HurtBox*)getComponent(ecs, e, hurtBoxId);
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

#endif