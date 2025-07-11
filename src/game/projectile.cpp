#include "projectile.hpp"
#include "boss.hpp"
#include "components.hpp"
#include "core.hpp"

void systemProjectileHit(Ecs* ecs){
    auto entitiesA = view(ecs, ProjectileTag, HitBox);
    auto entitiesB = view(ecs, HurtBox, EnemyTag);

    for(Entity entityA : entitiesA){
        HitBox* boxAent= getComponent(ecs, entityA, HitBox);
        //TransformComponent* tA= getComponent(ecs, entityA, TransformComponent);
        for(Entity entityB : entitiesB){
            if(entityA == entityB) continue; //skip self collision

            HurtBox* boxBent = getComponent(ecs, entityB, HurtBox);
            //TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            //Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
            //Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 

            //if(boxAent->area.active && boxBent->area.active && isColliding(&boxA, &boxB)){
            //if(onCollision(&boxA, &boxB) && !boxBent->invincible){
            if(beginCollision(entityA , entityB) && !boxBent->invincible){
                boxBent->health -= boxAent->dmg;
                if(!getComponent(ecs, entityA, ProjectileTag)->piercing){
                    destroyProjectile(ecs, entityA);
                }
                //LOGINFO("%d", boxBent->health);
                break;
            }
        }
    }
}

void systemCheckRange(Ecs* ecs){
    auto projectiles = view(ecs, ProjectileTag, TransformComponent);

    for(Entity e : projectiles){
        ProjectileTag* projectile = getComponent(ecs, e, ProjectileTag);
        TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        float distance = glm::length(projectile->initialPos - transform->position);
        if(distance > projectile->range){
            destroyProjectile(ecs, e);
            break;
        }
    }
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