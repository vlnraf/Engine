#include "projectile.hpp"
#include "gamekit/colliders.hpp"

Entity createProjectile(Ecs* ecs, EngineState* engine, glm::vec3 pos, glm::vec2 dir){
    Entity projectile = createEntity(ecs);

    TransformComponent transform = {    
        .position = pos,
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    SpriteComponent sprite = {
        .texture = getTexture(engine->textureManager, "default"),
        .index = {0,0},
        .size = {10, 10},
        .ySort = true,
        .layer = 1.0f
    };

    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size {10,10}};

    VelocityComponent velocity = {.vel = {50, 50}};
    DirectionComponent direction = {.dir = dir};
    ProjectileTag projectileTag = {};

    std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));

    pushComponent(ecs, projectile, TransformComponent, &transform);
    pushComponent(ecs, projectile, SpriteComponent, &sprite);
    pushComponent(ecs, projectile, VelocityComponent, &velocity);
    pushComponent(ecs, projectile, DirectionComponent, &direction);
    pushComponent(ecs, projectile, Box2DCollider, &collider);
    pushComponent(ecs, projectile, ProjectileTag, &projectileTag);

    return projectile;
}

void destroyProjectile(Ecs* ecs, Entity entity){
    removeEntity(ecs, entity);
}