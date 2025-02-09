#include "projectile.hpp"
#include "gamekit/colliders.hpp"
#include "hitbox.hpp"

Entity createProjectile(Ecs* ecs, EngineState* engine, glm::vec3 pos, glm::vec2 dir){
    Entity projectile = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture(engine->textureManager, "default"),
        .index = {0,0},
        .size = {5, 5},
        .ySort = true,
        .layer = 1.0f
    };

    TransformComponent transform = {    
        .position = {pos.x - (sprite.size.x / 2), pos.y - (sprite.size.y / 2), pos.z},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size = sprite.size};

    VelocityComponent velocity = {.vel = {300, 300}};
    DirectionComponent direction = {.dir = dir};
    ProjectileTag projectileTag = {};
    HitBox hitbox = {.dmg = 1, .area = collider};

    std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));

    pushComponent(ecs, projectile, TransformComponent, &transform);
    pushComponent(ecs, projectile, SpriteComponent, &sprite);
    pushComponent(ecs, projectile, VelocityComponent, &velocity);
    pushComponent(ecs, projectile, DirectionComponent, &direction);
    pushComponent(ecs, projectile, Box2DCollider, &collider);
    pushComponent(ecs, projectile, ProjectileTag, &projectileTag);
    pushComponent(ecs, projectile, HitBox, &hitbox);

    return projectile;
}

void destroyProjectile(Ecs* ecs, Entity entity){
    removeEntity(ecs, entity);
}