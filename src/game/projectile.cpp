#include "projectile.hpp"
#include "boss.hpp"
#include "core.hpp"

#include "components.hpp"

ECS_DECLARE_COMPONENT(ProjectileTag)

void systemProjectileHit(Ecs* ecs){
    EntityArray entitiesA = view(ecs, ECS_TYPE(HitBox));
    EntityArray entitiesB = view(ecs, ECS_TYPE(HurtBox), ECS_TYPE(EnemyTag));

    //for(Entity entityA : entitiesA){
    for(size_t i = 0; i < entitiesA.count; i++){
        Entity entityA = entitiesA.entities[i];
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, HitBox);
        //TransformComponent* tA= getComponent(ecs, entityA, TransformComponent);
        //for(Entity entityB : entitiesB){
        for(size_t i = 0; i < entitiesB.count; i++){
            Entity entityB = entitiesB.entities[i];
            if(entityA == entityB) continue; //skip self collision
            if(hasComponent(ecs, entityA, EnemyTag) && hasComponent(ecs, entityB, EnemyTag)) continue;

            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, HurtBox);
            //TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            //Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
            //Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 

            //if(boxAent->area.active && boxBent->area.active && isColliding(&boxA, &boxB)){
            //if(onCollision(&boxA, &boxB) && !boxBent->invincible){
            if(beginCollision(entityA , entityB) && !boxBent->invincible){
                boxBent->health -= boxAent->dmg;
                if(hasComponent(ecs, entityA, ProjectileTag) && !((ProjectileTag*)getComponent(ecs, entityA, ProjectileTag))->piercing){
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
    EntityArray projectiles = view(ecs, ECS_TYPE(ProjectileTag), ECS_TYPE(TransformComponent));

    //for(Entity e : projectiles){
    for(size_t i = 0; i < projectiles.count; i++){
        Entity e = projectiles.entities[i];
        ProjectileTag* projectile = (ProjectileTag*) getComponent(ecs, e, ProjectileTag);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
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


    pushComponent(ecs, projectile, TransformComponent, &transform);
    pushComponent(ecs, projectile, SpriteComponent, &sprite);
    pushComponent(ecs, projectile, VelocityComponent, &velocity);
    pushComponent(ecs, projectile, DirectionComponent, &direction);
    //pushComponent(ecs, projectile, Box2DCollider, &collider);
    pushComponent(ecs, projectile, ProjectileTag, &projectileTag);
    pushComponent(ecs, projectile, HitBox, &hitbox);

    return projectile;
}

void destroyProjectile(Ecs* ecs, Entity entity){
    removeEntity(ecs, entity);
}