#include "player.hpp"
#include "gamekit/colliders.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState currentState = IDLE;
PlayerState nextState = IDLE;

void inputPlayerSystem(Ecs* ecs, Input* input){
    auto entities = view(ecs, PlayerTag, DirectionComponent);
    //for(Entity e : entities){
    //    DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, e, ECS_DIRECTION);
    //    if((fabs(input->gamepad.leftX) > 0.2) || (fabs(input->gamepad.leftY > 0.2))){
    //        currentState = WALKING;
    //        direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
    //    }else{
    //        currentState = IDLE;
    //        direction->dir = {0, 0};
    //    }
    //}
    DirectionComponent* direction = getComponentVector(ecs, DirectionComponent);
    for(Entity e : entities){
        if((fabs(input->gamepad.leftX) > 0.1) || (fabs(input->gamepad.leftY) > 0.1)){
            currentState = WALKING;
            direction[e].dir = {input->gamepad.leftX, input->gamepad.leftY};
        }else{
            currentState = IDLE;
            direction[e].dir = {0, 0};
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

    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size {16,16}};

    VelocityComponent velocity = {.vel = {100, 100}};
    DirectionComponent direction = {.dir = {0, 0}};

    PlayerTag playerTag = {};
    std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));

    //pushComponent(ecs, player, ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //pushComponent(ecs, player, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    //pushComponent(ecs, player, ECS_PLAYER_TAG, &playerTag, sizeof(PlayerTag));
    //pushComponent(ecs, player, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
    //pushComponent(ecs, player, ECS_DIRECTION, &direction, sizeof(DirectionComponent));

    pushComponent(ecs, player, TransformComponent, &transform);
    pushComponent(ecs, player, SpriteComponent, &sprite);
    pushComponent(ecs, player, PlayerTag, &playerTag);
    pushComponent(ecs, player, VelocityComponent, &velocity);
    pushComponent(ecs, player, DirectionComponent, &direction);
    pushComponent(ecs, player, Box2DCollider, &collider);

    return player;
}