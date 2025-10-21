#include "projectile.hpp"
#include "boss.hpp"
#include "components.hpp"
#include "core.hpp"
#include "componentIds.hpp"


void systemProjectileHit(Ecs* ecs){
    EntityArray entitiesA = view(ecs, (size_t[]){hitBoxId}, 1);
    EntityArray entitiesB = view(ecs, (size_t[]){hurtBoxId, enemyTagId}, 2);

    //for(Entity entityA : entitiesA){
    for(size_t i = 0; i < entitiesA.count; i++){
        Entity entityA = entitiesA.entities[i];
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, hitBoxId);
        //TransformComponent* tA= getComponent(ecs, entityA, TransformComponent);
        //for(Entity entityB : entitiesB){
        for(size_t i = 0; i < entitiesB.count; i++){
            Entity entityB = entitiesB.entities[i];
            if(entityA == entityB) continue; //skip self collision
            if(hasComponent(ecs, entityA, enemyTagId) && hasComponent(ecs, entityB, enemyTagId)) continue;

            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, hurtBoxId);
            //TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            //Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
            //Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 

            //if(boxAent->area.active && boxBent->area.active && isColliding(&boxA, &boxB)){
            //if(onCollision(&boxA, &boxB) && !boxBent->invincible){
            if(beginCollision(entityA , entityB) && !boxBent->invincible){
                boxBent->health -= boxAent->dmg;
                if(hasComponent(ecs, entityA, projectileTagId) && !((ProjectileTag*)getComponent(ecs, entityA, projectileTagId))->piercing){
                    destroyProjectile(ecs, entityA);
                }
                //LOGINFO("%d", boxBent->health);
                break;
            }
        }
    }
}

void systemCheckRange(Ecs* ecs){
    PROFILER_START();
    EntityArray projectiles = view(ecs, (size_t[]){projectileTagId, transformComponentId}, 2);

    //for(Entity e : projectiles){
    for(size_t i = 0; i < projectiles.count; i++){
        Entity e = projectiles.entities[i];
        ProjectileTag* projectile = (ProjectileTag*) getComponent(ecs, e, projectileTagId);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, transformComponentId);
        float distance = glm::length(projectile->initialPos - transform->position);
        if(distance > projectile->range){
            destroyProjectile(ecs, e);
            break;
        }
    }
    PROFILER_END();
}


Entity createProjectile(Ecs* ecs, glm::vec3 pos, glm::vec2 dir, float dmg, float range, float radius, bool piercing){
    Entity projectile = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture("default"),
        .index = {0,0},
        .size = {radius, radius},
        .ySort = true,
        .layer = 1.0f
    };
    //sprite.textureName = "default";
    //std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));

    TransformComponent transform = {    
        .position = {pos.x - (sprite.size.x / 2), pos.y - (sprite.size.y / 2), pos.z},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size = sprite.size};
    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size = sprite.size};

    VelocityComponent velocity = {.vel = {300, 300}};
    DirectionComponent direction = {.dir = dir};
    ProjectileTag projectileTag = {.initialPos = pos, .range = range, .piercing = piercing};
    HitBox hitbox = {.dmg = dmg, .offset = {0,0}, .size = sprite.size};


    pushComponent(ecs, projectile, transformComponentId, &transform);
    pushComponent(ecs, projectile, spriteComponentId, &sprite);
    pushComponent(ecs, projectile, velocityComponentId, &velocity);
    pushComponent(ecs, projectile, directionComponentId, &direction);
    //pushComponent(ecs, projectile, Box2DCollider, &collider);
    pushComponent(ecs, projectile, projectileTagId, &projectileTag);
    pushComponent(ecs, projectile, hitBoxId, &hitbox);

    return projectile;
}

void destroyProjectile(Ecs* ecs, Entity entity){
    removeEntity(ecs, entity);
}