#include "player.hpp"
#include "gamekit/colliders.hpp"

#include "projectile.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState currentState = IDLE;
PlayerState nextState = IDLE;

void inputPlayerSystem(Ecs* ecs, EngineState* engine, Input* input){
    auto entities = view(ecs, PlayerTag, DirectionComponent, TransformComponent, Box2DCollider);
    for(Entity e : entities){
        DirectionComponent* direction = getComponent(ecs, e, DirectionComponent);
        TransformComponent* t = getComponent(ecs, e, TransformComponent);
        Box2DCollider* b = getComponent(ecs, e, Box2DCollider);
        if((fabs(input->gamepad.leftX) > 0.1) || (fabs(input->gamepad.leftY) > 0.1)){
            currentState = WALKING;
            direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
        }else{
            currentState = IDLE;
            direction->dir = {0, 0};
        }
        //if(isjustpressedgamepad(&input->gamepad, gamepad_button_x)){
        if(isJustPressedGamepad(&input->gamepad, GAMEPAD_BUTTON_X)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.x -= 20;
            createProjectile(ecs, engine, center, {-1, 0});
        }else if(isJustPressedGamepad(&input->gamepad, GAMEPAD_BUTTON_Y)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.y += 20;
            createProjectile(ecs, engine, center, {0, 1});
        }else if(isJustPressedGamepad(&input->gamepad, GAMEPAD_BUTTON_B)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.x +=20;
            createProjectile(ecs, engine, center, {1, 0});
        }else if(isJustPressedGamepad(&input->gamepad, GAMEPAD_BUTTON_A)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.y -= 20;
            createProjectile(ecs, engine, center, {0, -1});
        }
    }
}

Entity createPlayer(Ecs* ecs, EngineState* engine, OrtographicCamera camera) {
    Entity player = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture("default"),
        .index = {0,0},
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f
    };
    
    TransformComponent transform = {    
        .position = {(camera.width / 2) - (sprite.size.x / 2), 50.0f, 0.0f},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size {16,16}};
    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size {16,16}};

    VelocityComponent velocity = {.vel = {150, 150}};
    DirectionComponent direction = {.dir = {0, 0}};

    PlayerTag playerTag = {};
    HurtBox hurtBox = {.health = 1, .offset=collider.offset, .size = collider.size};
    std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));

    pushComponent(ecs, player, TransformComponent, &transform);
    pushComponent(ecs, player, SpriteComponent, &sprite);
    pushComponent(ecs, player, PlayerTag, &playerTag);
    pushComponent(ecs, player, VelocityComponent, &velocity);
    pushComponent(ecs, player, DirectionComponent, &direction);
    pushComponent(ecs, player, Box2DCollider, &collider);
    pushComponent(ecs, player, HurtBox, &hurtBox);

    return player;
}