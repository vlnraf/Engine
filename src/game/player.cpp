#include "player.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState currentState = IDLE;
PlayerState nextState = IDLE;

void inputPlayerSystem(Ecs* ecs, Input* input){
    auto entities = view(ecs, {ECS_PLAYER_TAG, ECS_DIRECTION});
    for(Entity e : entities){
        DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, e, ECS_DIRECTION);
        if((fabs(input->gamepad.leftX) > 0.2) || (fabs(input->gamepad.leftY > 0.2))){
            currentState = WALKING;
            direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
        }else{
            currentState = IDLE;
            direction->dir = {0, 0};
        }
    }
}

Entity createPlayer(Ecs* ecs, EngineState* engine) {
    Entity player = createEntity(ecs);

    TransformComponent transform = {    
        .position = {50.0f, 50.0f, 0.0f},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    SpriteComponent sprite = {
        .texture = getTexture(engine->textureManager, "default"),
        .index = {0,0},
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f
    };

    VelocityComponent velocity = {.vel = {100, 100}};
    DirectionComponent direction = {.dir = {0, 0}};

    PlayerTag playerTag = {};
    std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));

    pushComponent(ecs, player, ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    pushComponent(ecs, player, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    pushComponent(ecs, player, ECS_PLAYER_TAG, &playerTag, sizeof(PlayerTag));
    pushComponent(ecs, player, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
    pushComponent(ecs, player, ECS_DIRECTION, &direction, sizeof(DirectionComponent));

    return player;
}