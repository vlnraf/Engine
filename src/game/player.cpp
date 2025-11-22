#include "player.hpp"

#include "projectile.hpp"
#include "vampireclone.hpp"
#include "weapon.hpp"

#include "components.hpp"

//NOTE: make it a component who stores entity states???
enum PlayerState{
    IDLE,
    WALKING,
    ATTAKING
};
PlayerState playercurrState = IDLE;
PlayerState playerNextState = IDLE;

void inputPlayerSystem(Ecs* ecs, Input* input, float dt){
    EntityArray entities = view(ecs, ECS_TYPE(PlayerTag), ECS_TYPE(DirectionComponent), ECS_TYPE(TransformComponent), ECS_TYPE(Box2DCollider), ECS_TYPE(SpriteComponent), ECS_TYPE(AnimationComponent));
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i ++){
        Entity e = entities.entities[i];
        DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, e, DirectionComponent);
        AnimationComponent* anim = (AnimationComponent*) getComponent(ecs, e, AnimationComponent);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, e, SpriteComponent);
        PlayerTag* playerTag = (PlayerTag*) getComponent(ecs, e, PlayerTag);
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

        if(isPressed(KEYS::W)){
            direction->dir.y += 1;
            strncpy(anim->animationId, "player-walkTop", sizeof(anim->animationId));
        }
        if(isPressed(KEYS::S)){
            direction->dir.y -= 1;
            strncpy(anim->animationId, "player-walkBottom", sizeof(anim->animationId));
        }
        if(isPressed(KEYS::A)){
            direction->dir.x -= 1;
            sprite->flipX = true;
            strncpy(anim->animationId, "player-walkLeft", sizeof(anim->animationId));
        }
        if(isPressed(KEYS::D)){
            direction->dir.x += 1;
            sprite->flipX = false;
            strncpy(anim->animationId, "player-walkRight", sizeof(anim->animationId));
        }
        if(glm::length(direction->dir) != 0){
            direction->dir = glm::normalize(direction->dir);
        }
    }
}


Entity createPlayer(Ecs* ecs, OrtographicCamera camera) {
    Entity player = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTextureByName("idle-walk"),
        .sourceRect = {.pos = {0,0}, .size = {16, 16}},
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f,
        .visible = true
    };
    //sprite.textureName = "idle-walk";
    //std::strncpy(sprite.textureName, "default", sizeof(sprite.textureName));
    
    TransformComponent transform = {    
        .position = {(camera.width / 2) - (sprite.size.x / 2), 50.0f, 0.0f},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {1, -(sprite.size.y / 2)+ (4/2)}, .size = {14,4}};

    VelocityComponent velocity = {.vel = {150, 150}};
    DirectionComponent direction = {.dir = {0, 0}};

    PlayerTag playerTag = {};

    glm::vec2 tileSize = {16, 16};
    registryAnimation("player-idleRight", 4, (uint16_t)0, tileSize, true);
    registryAnimation("player-idleLeft", 4, (uint16_t)0, tileSize, true);
    registryAnimation("player-idleBottom", 4, (uint16_t)1, tileSize, true);
    registryAnimation("player-idleTop", 4, (uint16_t)2, tileSize, true);
    registryAnimation("player-walkRight", 8, (uint16_t)3, tileSize, true);
    registryAnimation("player-walkLeft", 8, (uint16_t)3, tileSize, true);
    registryAnimation("player-walkBottom", 8, (uint16_t)4, tileSize, true);
    registryAnimation("player-walkTop", 8, (uint16_t)5, tileSize, true);

    AnimationComponent anim = {};
    strncpy(anim.animationId, "player-idleTop", sizeof(anim.animationId));

    PersistentTag persistent = {};
    pushComponent(ecs, player, PersistentTag, &persistent);

    InputComponent inputComponent = {.fire = true, .direction = {0,0}};
    HealthComponent health = {.hp = 100};

    pushComponent(ecs, player, TransformComponent, &transform);
    pushComponent(ecs, player, SpriteComponent, &sprite);
    pushComponent(ecs, player, PlayerTag, &playerTag);
    pushComponent(ecs, player, VelocityComponent, &velocity);
    pushComponent(ecs, player, DirectionComponent, &direction);
    pushComponent(ecs, player, Box2DCollider, &collider);
    pushComponent(ecs, player, AnimationComponent, &anim);
    pushComponent(ecs, player, InputComponent, &inputComponent);
    pushComponent(ecs, player, HealthComponent, &health);

    ExperienceComponent exp = {.currentXp = 0.0f};
    pushComponent(ecs, player, ExperienceComponent, &exp);
    HasWeaponComponent hasWeapon = {};
    pushComponent(ecs, player, HasWeaponComponent, &hasWeapon);

    Entity hurtbox = createEntity(ecs);
    Box2DCollider hurtboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {1,0}, .size {14,14}, .isTrigger = true};
    pushComponent(ecs, hurtbox, Box2DCollider, &hurtboxCollider);
    TransformComponent hurtboxTransform = transform;
    pushComponent(ecs, hurtbox, TransformComponent, &hurtboxTransform);
    Parent parent = {.entity = player};
    pushComponent(ecs, hurtbox, Parent, &parent);
    HurtboxTag hurtboxTag = {};
    pushComponent(ecs, hurtbox, HurtboxTag, &hurtboxTag);
    pushComponent(ecs, hurtbox, PersistentTag, &persistent);

    return player;
}
