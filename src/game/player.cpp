#include "player.hpp"

#include "projectile.hpp"
#include "components.hpp"
#include "vampireclone.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState currentState = IDLE;
PlayerState nextState = IDLE;

void inputPlayerSystem(Ecs* ecs, EngineState* engine, Input* input){
    auto entities = view(ecs, PlayerTag, DirectionComponent, TransformComponent, Box2DCollider, SpriteComponent, AnimationComponent);
    for(Entity e : entities){
        DirectionComponent* direction = getComponent(ecs, e, DirectionComponent);
        TransformComponent* t = getComponent(ecs, e, TransformComponent);
        Box2DCollider* b = getComponent(ecs, e, Box2DCollider);
        AnimationComponent* anim = getComponent(ecs, e, AnimationComponent);
        SpriteComponent* sprite = getComponent(ecs, e, SpriteComponent);
        if((fabs(input->gamepad.leftX) > 0.1) || (fabs(input->gamepad.leftY) > 0.1)){
            currentState = WALKING;
            direction->dir = {input->gamepad.leftX, input->gamepad.leftY};

            //animation transition
            if(input->gamepad.leftX < -0.3){
                sprite->flipX = true;
                strncpy(anim->animationId, "player-walkLeft", sizeof(anim->animationId));
            }else if(input->gamepad.leftX > 0.3){
                sprite->flipX = false;
                strncpy(anim->animationId, "player-walkRight", sizeof(anim->animationId));
            }else if(input->gamepad.leftY > 0.3){
                strncpy(anim->animationId, "player-walkTop", sizeof(anim->animationId));
            }else if(input->gamepad.leftY < -0.3){
                strncpy(anim->animationId, "player-walkBottom", sizeof(anim->animationId));
            }
        }else{
            currentState = IDLE;
            strncpy(anim->animationId, "player-idleBottom", sizeof(anim->animationId));
            direction->dir = {0, 0};
        }
        //if(isjustpressedgamepad(&input->gamepad, gamepad_button_x)){
        if(isJustPressedGamepad(GAMEPAD_BUTTON_X)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.x -= 20;
            createProjectile(ecs, engine, center, {-1, 0});
        }else if(isJustPressedGamepad(GAMEPAD_BUTTON_Y)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.y += 20;
            createProjectile(ecs, engine, center, {0, 1});
        }else if(isJustPressedGamepad(GAMEPAD_BUTTON_B)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.x +=20;
            createProjectile(ecs, engine, center, {1, 0});
        }else if(isJustPressedGamepad(GAMEPAD_BUTTON_A)){
            glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
            center.y -= 20;
            createProjectile(ecs, engine, center, {0, -1});
        }
    }
}

Entity createPlayer(Ecs* ecs, EngineState* engine, OrtographicCamera camera) {
    Entity player = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture("idle-walk"),
        .index = {0,0},
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f
    };
    //sprite.textureName = "idle-walk";
    //std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));
    
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
    HurtBox hurtBox = {.health = 100, .offset=collider.offset, .size = collider.size};

    registryAnimation("player-idleRight", 4, 0, true);
    registryAnimation("player-idleLeft", 4, 0, true);
    registryAnimation("player-idleBottom", 4, 1, true);
    registryAnimation("player-idleTop", 4, 2, true);
    registryAnimation("player-walkRight", 8, 3, true);
    registryAnimation("player-walkLeft", 8, 3, true);
    registryAnimation("player-walkBottom", 8, 4, true);
    registryAnimation("player-walkTop", 8, 5, true);

    //Animation anim = {};
    //anim.frames = 4;
    //for(int i = 0; i < 4; i++){
    //    anim.indices[i] = {i, 0};
    //}
    //anim.frameDuration = 1.0f / 4;
    //anim.loop = true;

    AnimationComponent anim = {};
    //anim.animationId = "player-idleTop";
    strncpy(anim.animationId, "player-idleTop", sizeof(anim.animationId));

    pushComponent(ecs, player, TransformComponent, &transform);
    pushComponent(ecs, player, SpriteComponent, &sprite);
    pushComponent(ecs, player, PlayerTag, &playerTag);
    pushComponent(ecs, player, VelocityComponent, &velocity);
    pushComponent(ecs, player, DirectionComponent, &direction);
    pushComponent(ecs, player, Box2DCollider, &collider);
    pushComponent(ecs, player, HurtBox, &hurtBox);
    pushComponent(ecs, player, AnimationComponent, &anim);

    ExperienceComponent exp = {.currentXp = 0.0f, .xpDrop = 0.0f};
    pushComponent(ecs, player, ExperienceComponent, &exp);

    return player;
}