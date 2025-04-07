#include "telegraphattack.hpp"
#include "lifetime.hpp"

Entity createTelegraphAttack(Ecs* ecs, EngineState* engine, OrtographicCamera camera, glm::vec3 pos){
    Entity telegraph = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture("default"),
        .index = {0,0},
        .size = {15, 15},
        .color = {1, 0, 0, 0.5},
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

    LifeTime lifetime = {.time = 0, .endTime = 0.5f};


    pushComponent(ecs, telegraph, TransformComponent, &transform);
    pushComponent(ecs, telegraph, SpriteComponent, &sprite);
    pushComponent(ecs, telegraph, LifeTime, &lifetime);

    return telegraph;
}