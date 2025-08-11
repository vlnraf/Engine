#include "spike.hpp"
#include "lifetime.hpp"

void systemSpikeHit(Ecs* ecs){
    EntityArray entitiesA = view(ecs, SpikeTag, HitBox);
    EntityArray entitiesB = view(ecs, PlayerTag, HurtBox);

    //for(Entity entityA : entitiesA){
    for(size_t i = 0; i < entitiesA.count; i++){
        Entity entityA = entitiesA.entities[i];
        HitBox* boxAent= getComponent(ecs, entityA, HitBox);
        //for(Entity entityB : entitiesB){
        for(size_t i = 0; i < entitiesB.count; i++){
            Entity entityB = entitiesB.entities[i];
            if(entityA == entityB) continue; //skip self collision

            HurtBox* boxBent = getComponent(ecs, entityB, HurtBox);
            if(beginCollision(entityA , entityB) && !boxBent->invincible){
                boxBent->health -= boxAent->dmg;
                LOGINFO("%d", boxBent->health);
                break;
            }
        }
    }
}

Entity createSpike(Ecs* ecs, glm::vec3 pos){
    Entity spike = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture("default"),
        .index = {0,0},
        .size = {15, 15},
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

    SpikeTag spikeTag = {};
    HitBox hitbox = {.dmg = 1, .offset = {0,0}, .size = sprite.size};
    LifeTime lifetime = {.time = 0, .endTime = 0.5f};


    pushComponent(ecs, spike, TransformComponent, &transform);
    pushComponent(ecs, spike, SpriteComponent, &sprite);
    pushComponent(ecs, spike, SpikeTag, &spikeTag);
    pushComponent(ecs, spike, HitBox, &hitbox);
    pushComponent(ecs, spike, LifeTime, &lifetime);

    return spike;
}