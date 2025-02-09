#include "boss.hpp"
#include "gamekit/colliders.hpp"
#include "hitbox.hpp"

void bossAiSystem(Ecs* ecs, float dt){
    auto bosses = view(ecs, BossTag, Box2DCollider, DirectionComponent, TransformComponent, VelocityComponent);
    auto players = view(ecs, PlayerTag);
    //NOTE: i am sure it's only one right now
    Entity player = players[0];

    float dashCd = 2;
    static float dash = 0;

    for(Entity b : bosses){
        Box2DCollider* boxBoss = getComponent(ecs, b, Box2DCollider);
        DirectionComponent* dirBoss = getComponent(ecs, b, DirectionComponent);
        TransformComponent* tBoss = getComponent(ecs, b, TransformComponent);
        VelocityComponent* vBoss = getComponent(ecs, b, VelocityComponent);

        Box2DCollider* boxPlayer = getComponent(ecs, player, Box2DCollider);
        TransformComponent* tPlayer = getComponent(ecs, player, TransformComponent);
        if(dash > dashCd){
            dirBoss->dir = {tPlayer->position.x - tBoss->position.x, tPlayer->position.y - tBoss->position.y};
            dirBoss->dir = glm::normalize(dirBoss->dir);
            boxBoss->onCollision = false;
            vBoss->vel = {300, 300};
            dash = 0;
        }
    }
    dash += dt;
}

void bossActiveHurtBoxSystem(Ecs* ecs){
    auto bosses = view(ecs, Box2DCollider, HurtBox, BossTag, DirectionComponent);
    //NOTE: i am sure it's only one right now
    for(Entity b : bosses){
        HurtBox* hurtBox = getComponent(ecs, b, HurtBox);
        Box2DCollider* box = getComponent(ecs, b, Box2DCollider);
        DirectionComponent* dir = getComponent(ecs, b, DirectionComponent);
        if(box->onCollision == true){
            dir->dir = {0,0};
            hurtBox->area.active = true;
        }else{
            hurtBox->area.active = false;
        }
    }
}

Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera){
    Entity boss = createEntity(ecs);
    SpriteComponent sprite = {
        .texture = getTexture(engine->textureManager, "default"),
        .index = {0,0},
        .size = {50, 50},
        .ySort = true,
        .layer = 1.0f
    };
    TransformComponent transform = {    
        .position = {(camera.width / 2) - (sprite.size.x / 2), (camera.height - (camera.height / 3)) - sprite.size.y , 0.0f},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };
    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size = sprite.size};
    sprite.color = {0,0,1,1};
    BossTag bossTag = {};
    VelocityComponent velocity = {.vel = {100, 100}};
    DirectionComponent direction = {.dir = {0, 0}};
    HitBox hitbox = {.dmg = 1, .area = collider};
    Box2DCollider hurtBoxCollider = collider;
    hurtBoxCollider.active = false;
    hurtBoxCollider.size = {10, 10};
    hurtBoxCollider.offset = {collider.size.x / 2 - hurtBoxCollider.size.x/2, collider.size.y /2 - hurtBoxCollider.size.y/2};
    HurtBox hurtbox = {.health = 3, .area = hurtBoxCollider};

    pushComponent(ecs, boss, TransformComponent, &transform);
    pushComponent(ecs, boss, VelocityComponent, &velocity);
    pushComponent(ecs, boss, DirectionComponent, &direction);
    pushComponent(ecs, boss, SpriteComponent, &sprite);
    pushComponent(ecs, boss, Box2DCollider, &collider);
    pushComponent(ecs, boss, BossTag, &bossTag);
    pushComponent(ecs, boss, HitBox, &hitbox);
    pushComponent(ecs, boss, HurtBox, &hurtbox);


    return boss;
}