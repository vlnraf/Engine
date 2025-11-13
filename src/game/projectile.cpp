#include "projectile.hpp"
#include "core.hpp"

#include "components.hpp"
#include "weapon.hpp"

ECS_DECLARE_COMPONENT(ProjectileTag)

void systemProjectileHit(Ecs* ecs){
    TriggerEventArray* events = getTriggerEnterEvents();
    for(size_t i = 0; i < events->count; i++){
        CollisionEvent event = events->item[i];
        Entity entityA = event.entityA.entity;
        Entity entityB = event.entityB.entity;
        if(hasComponent(ecs, entityA, HitboxTag) && hasComponent(ecs, entityB, HurtboxTag)){
            Parent* parentA = getComponent(ecs, entityA, Parent);
            Parent* parentB = getComponent(ecs, entityB, Parent);
            //Projectiles cant hit the player
            if(hasComponent(ecs, parentA->entity, ProjectileTag) && hasComponent(ecs, parentB->entity, PlayerTag)) continue;
            //skip enemy enemy check
            if(hasComponent(ecs, parentA->entity, EnemyTag) && hasComponent(ecs, parentB->entity, EnemyTag)) continue;
            if(hasComponent(ecs, parentA->entity, ExplosionTag) && hasComponent(ecs, parentB->entity, PlayerTag)) continue;
            if(parentA->entity == parentB->entity) continue;
            if(!parentA && !parentB) continue;
            HealthComponent* health = getComponent(ecs, parentB->entity, HealthComponent);
            DamageComponent* damage = getComponent(ecs, parentA->entity, DamageComponent);
            if(health){
                LOGINFO("%f", health->hp);
                health->hp -= damage->dmg;
            }
            if(hasComponent(ecs, parentA->entity, ProjectileTag)){
                destroyProjectile(ecs, parentA->entity);
            }
        }else if(hasComponent(ecs, entityA, HurtboxTag) && hasComponent(ecs, entityB, HitboxTag)){
            Parent* parentA = getComponent(ecs, entityA, Parent);
            Parent* parentB = getComponent(ecs, entityB, Parent);
            //Projectiles cant hit the player
            if(hasComponent(ecs, parentB->entity, ProjectileTag) && hasComponent(ecs, parentA->entity, PlayerTag)) continue;
            //skip enemy enemy check
            if(hasComponent(ecs, parentB->entity, EnemyTag) && hasComponent(ecs, parentA->entity, EnemyTag)) continue;
            if(hasComponent(ecs, parentB->entity, ExplosionTag) && hasComponent(ecs, parentA->entity, PlayerTag)) continue;
            if(parentA->entity == parentB->entity) continue;
            if(!parentA && !parentB) continue;
            HealthComponent* health = getComponent(ecs, parentA->entity, HealthComponent);
            DamageComponent* damage = getComponent(ecs, parentB->entity, DamageComponent);
            if(health){
                LOGINFO("%f", health->hp);
                health->hp -= damage->dmg;
            }
            if(hasComponent(ecs, parentB->entity, ProjectileTag)){
                destroyProjectile(ecs, parentB->entity);
            }
        }
    }
}

void systemCheckRange(Ecs* ecs){
    EntityArray projectiles = view(ecs, ECS_TYPE(ProjectileTag), ECS_TYPE(TransformComponent));

    for(size_t i = 0; i < projectiles.count; i++){
        Entity e = projectiles.entities[i];
        ProjectileTag* projectile = (ProjectileTag*) getComponent(ecs, e, ProjectileTag);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
        float distance = glm::length(projectile->initialPos - transform->position);
        if(distance > projectile->range){
            destroyProjectile(ecs, e);
            //break;
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
        .position = {pos.x , pos.y, pos.z},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size = sprite.size};
    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size = sprite.size};

    VelocityComponent velocity = {.vel = {300, 300}};
    DirectionComponent direction = {.dir = dir};
    ProjectileTag projectileTag = {.initialPos = pos, .range = range, .piercing = piercing};
    DamageComponent damage = {.dmg = dmg};


    pushComponent(ecs, projectile, TransformComponent, &transform);
    pushComponent(ecs, projectile, SpriteComponent, &sprite);
    pushComponent(ecs, projectile, VelocityComponent, &velocity);
    pushComponent(ecs, projectile, DirectionComponent, &direction);
    //pushComponent(ecs, projectile, Box2DCollider, &collider);
    pushComponent(ecs, projectile, ProjectileTag, &projectileTag);
    pushComponent(ecs, projectile, DamageComponent, &damage);
    //HitBox hitbox = {.dmg = dmg, .offset = {0,0}, .size = sprite.size};
    //pushComponent(ecs, projectile, HitBox, &hitbox);

    Entity hitbox = createEntity(ecs);
    Box2DCollider hitboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {1,0}, .size {radius,radius}, .isTrigger = true};
    pushComponent(ecs, hitbox, Box2DCollider, &hitboxCollider);
    pushComponent(ecs, hitbox, TransformComponent, &transform);
    Parent parent = {.entity = projectile};
    pushComponent(ecs, hitbox, Parent, &parent);
    HitboxTag hitboxTag = {};
    pushComponent(ecs, hitbox, HitboxTag, &hitboxTag);

    return projectile;
}

void destroyProjectile(Ecs* ecs, Entity entity){
    Child* childs = getComponent(ecs, entity, Child);
    for(size_t j = 0; j < childs->count; j++){
        removeEntity(ecs, childs->entity[j]);
    }
    removeEntity(ecs, entity);
}