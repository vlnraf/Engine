#include "player.hpp"

#include "projectile.hpp"
#include "components.hpp"
#include "vampireclone.hpp"
#include "weapon.hpp"
#include "projectx.hpp"
#include "componentIds.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState playercurrState = IDLE;
PlayerState playerNextState = IDLE;

void inputPlayerSystem(Ecs* ecs, Input* input, float dt){
    EntityArray entities = view(ecs, (size_t[]){playerTagId, directionComponentId, transformComponentId, box2DColliderId, spriteComponentId, animationComponentId}, 6);
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i ++){
        Entity e = entities.entities[i];
        DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, e, directionComponentId);
        //TransformComponent* t = getComponent(ecs, e, TransformComponent);
        //Box2DCollider* b = getComponent(ecs, e, Box2DCollider);
        AnimationComponent* anim = (AnimationComponent*) getComponent(ecs, e, animationComponentId);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, e, spriteComponentId);
        PlayerTag* playerTag = (PlayerTag*) getComponent(ecs, e, playerTagId);
        //HasWeaponComponent* hasWeaponComponent = getComponent(ecs, e, HasWeaponComponent);
        InputComponent* inputComponent = (InputComponent*) getComponent(ecs, e, inputComponentId);
        playerTag->projectileCooldown += dt;
        if((fabs(input->gamepad.leftX) > 0.1) || (fabs(input->gamepad.leftY) > 0.1)){
            playercurrState = WALKING;
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
            playercurrState = IDLE;
            strncpy(anim->animationId, "player-idleBottom", sizeof(anim->animationId));
            direction->dir = {0, 0};
        }
        //if(isjustpressedgamepad(&input->gamepad, gamepad_button_x)){
        inputComponent->fire = isPressedGamepad(GAMEPAD_BUTTON_X) || isPressedGamepad(GAMEPAD_BUTTON_Y) || isPressedGamepad(GAMEPAD_BUTTON_B) || isPressedGamepad(GAMEPAD_BUTTON_A);
        if(isPressedGamepad(GAMEPAD_BUTTON_X)){
            inputComponent->direction = {-1, 0};
        }else if(isPressedGamepad(GAMEPAD_BUTTON_Y)){
            inputComponent->direction = {0, 1};
        }else if(isPressedGamepad(GAMEPAD_BUTTON_B)){
            inputComponent->direction = {1, 0};
        }else if(isPressedGamepad(GAMEPAD_BUTTON_A)){
            inputComponent->direction = {0, -1};
        }
        inputComponent->pickUp = isJustPressedGamepad(GAMEPAD_AXIS_RIGHT_TRIGGER);
    }
}


Entity createPlayer(Ecs* ecs, OrtographicCamera camera) {
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
        //.position = {100, 100, 0},
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

    //if(weaponType == WEAPON_GUN){
    //    Entity gun = createGun(ecs);
    //    HasWeaponComponent hasWeapon = {.weaponId = gun, .weaponType = WEAPON_GUN};
    //    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);
    //}else if(weaponType == WEAPON_SHOTGUN){
    //    Entity gun = createShotgun(ecs);
    //    HasWeaponComponent hasWeapon = {.weaponId = gun, .weaponType = WEAPON_SHOTGUN};
    //    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);
    //}else if(weaponType == WEAPON_SNIPER){
    //    Entity gun = createSniper(ecs);
    //    HasWeaponComponent hasWeapon = {.weaponId = gun, .weaponType = WEAPON_SNIPER};
    //    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);
    //}
    PersistentTag persistent = {};
    pushComponent(ecs, player, persistentTagId, &persistent);
    //Entity gun = createGun(ecs);
    //Entity gun = createShotgun(ecs);
    //HasWeaponComponent hasWeapon = {.weaponId = gun};
    //pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);

    InputComponent inputComponent = {.fire = true, .direction = {0,0}};

    pushComponent(ecs, player, transformComponentId, &transform);
    pushComponent(ecs, player, spriteComponentId, &sprite);
    pushComponent(ecs, player, playerTagId, &playerTag);
    pushComponent(ecs, player, velocityComponentId, &velocity);
    pushComponent(ecs, player, directionComponentId, &direction);
    pushComponent(ecs, player, box2DColliderId, &collider);
    pushComponent(ecs, player, hurtBoxId, &hurtBox);
    pushComponent(ecs, player, animationComponentId, &anim);
    pushComponent(ecs, player, inputComponentId, &inputComponent);

    ExperienceComponent exp = {.currentXp = 0.0f, .xpDrop = 0.0f};
    pushComponent(ecs, player, experienceComponentId, &exp);
    HasWeaponComponent hasWeapon = {};
    pushComponent(ecs, player, hasWeaponComponentId, &hasWeapon);

    return player;
}
