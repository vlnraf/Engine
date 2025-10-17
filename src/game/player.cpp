#include "player.hpp"

#include "projectile.hpp"
#include "components.hpp"
#include "vampireclone.hpp"
#include "weapon.hpp"
#include "projectx.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState playercurrState = IDLE;
PlayerState playerNextState = IDLE;

void inputPlayerSystem(Ecs* ecs, Input* input, float dt){
    EntityArray entities = view(ecs, PlayerTag, DirectionComponent, TransformComponent, Box2DCollider, SpriteComponent, AnimationComponent);
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i ++){
        Entity e = entities.entities[i];
        DirectionComponent* direction = getComponent(ecs, e, DirectionComponent);
        //TransformComponent* t = getComponent(ecs, e, TransformComponent);
        //Box2DCollider* b = getComponent(ecs, e, Box2DCollider);
        AnimationComponent* anim = getComponent(ecs, e, AnimationComponent);
        SpriteComponent* sprite = getComponent(ecs, e, SpriteComponent);
        PlayerTag* playerTag = getComponent(ecs, e, PlayerTag);
        //HasWeaponComponent* hasWeaponComponent = getComponent(ecs, e, HasWeaponComponent);
        InputComponent* inputComponent = getComponent(ecs, e, InputComponent);
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

    //if(weaponType == GUN){
    //    Entity gun = createGun(ecs);
    //    HasWeaponComponent hasWeapon = {.weaponId = gun, .weaponType = GUN};
    //    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);
    //}else if(weaponType == SHOTGUN){
    //    Entity gun = createShotgun(ecs);
    //    HasWeaponComponent hasWeapon = {.weaponId = gun, .weaponType = SHOTGUN};
    //    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);
    //}else if(weaponType == SNIPER){
    //    Entity gun = createSniper(ecs);
    //    HasWeaponComponent hasWeapon = {.weaponId = gun, .weaponType = SNIPER};
    //    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);
    //}
    PersistentTag persistent = {};
    pushComponent(ecs, player, PersistentTag, &persistent);
    //Entity gun = createGun(ecs);
    //Entity gun = createShotgun(ecs);
    //HasWeaponComponent hasWeapon = {.weaponId = gun};
    //pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);

    InputComponent inputComponent = {.fire = true, .direction = {0,0}};

    pushComponent(ecs, player, TransformComponent, &transform);
    pushComponent(ecs, player, SpriteComponent, &sprite);
    pushComponent(ecs, player, PlayerTag, &playerTag);
    pushComponent(ecs, player, VelocityComponent, &velocity);
    pushComponent(ecs, player, DirectionComponent, &direction);
    pushComponent(ecs, player, Box2DCollider, &collider);
    pushComponent(ecs, player, HurtBox, &hurtBox);
    pushComponent(ecs, player, AnimationComponent, &anim);
    pushComponent(ecs, player, InputComponent, &inputComponent);

    ExperienceComponent exp = {.currentXp = 0.0f, .xpDrop = 0.0f};
    pushComponent(ecs, player, ExperienceComponent, &exp);
    HasWeaponComponent hasWeapon = {};
    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);

    return player;
}
