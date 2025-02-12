#include "boss.hpp"
#include "gamekit/colliders.hpp"
#include "hitbox.hpp"
#include "projectile.hpp"

enum BossState{
    IDLE,
    DASHING,
    SMASHING,
    STUNNED
};

BossState currentState = IDLE;

void systemRespondBossHitStaticEntity(Ecs* ecs, const float dt){
    auto entitiesA = view(ecs, DirectionComponent, HurtBox, HitBox, BossTag);
    auto entitiesB = view(ecs, Box2DCollider);

    for(Entity entityA : entitiesA){
        HitBox* boxAent= getComponent(ecs, entityA, HitBox);
        HurtBox* boxAentHurt= getComponent(ecs, entityA, HurtBox);
        DirectionComponent* dirA = getComponent(ecs, entityA, DirectionComponent);
        TransformComponent* tA= getComponent(ecs, entityA, TransformComponent);
        for(Entity entityB : entitiesB){
            if(entityA == entityB) continue; //skip self collision

            Box2DCollider* boxBent = getComponent(ecs, entityB, Box2DCollider);
            TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
            Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 

            //if(boxAent->area.active && boxBent->area.active && isColliding(&boxA, &boxB)){
            //if(isColliding(&boxA, &boxB) && boxBent->type == Box2DCollider::STATIC){
            if(beginCollision(entityA, entityB) && boxBent->type == Box2DCollider::STATIC){
                boxAentHurt->invincible = false;
                dirA->dir = {0,0};
                currentState = STUNNED;
            }
        }
    }
}

void bossAiSystem(Ecs* ecs, EngineState* engine, float dt){
    auto bosses = view(ecs, BossTag, HurtBox, DirectionComponent, TransformComponent, VelocityComponent);
    auto players = view(ecs, PlayerTag);
    //NOTE: i am sure it's only one right now
    Entity player = players[0];
    srand(time(NULL));


    for(Entity b : bosses){
        static float dash = 0;
        float dashCd = 1.5;
        static float shoot = 0;
        float shootCd = 1.5;
        DirectionComponent* dirBoss = getComponent(ecs, b, DirectionComponent);
        TransformComponent* tBoss = getComponent(ecs, b, TransformComponent);
        VelocityComponent* vBoss = getComponent(ecs, b, VelocityComponent);
        HurtBox* hurtBoxBoss = getComponent(ecs, b, HurtBox);

        Box2DCollider* boxPlayer = getComponent(ecs, player, Box2DCollider);
        TransformComponent* tPlayer = getComponent(ecs, player, TransformComponent);
        BossState state = (BossState)((rand() % 2) + 1); //SKIPPING IDLE AND STUNNED right now
        if(state == IDLE){
            continue;
        }else if(state == DASHING){
            if(dash > dashCd){
                currentState = DASHING;
                Box2DCollider playerCollider =  calculateCollider(tPlayer, boxPlayer->offset, boxPlayer->size);
                glm::vec2 centerPlayer = getBoxCenter(&playerCollider);
                Box2DCollider bossCollider =  calculateCollider(tBoss, hurtBoxBoss->offset, hurtBoxBoss->size);
                glm::vec2 centerBoss= getBoxCenter(&bossCollider);
                dirBoss->dir = centerPlayer - centerBoss;
                dirBoss->dir = glm::normalize(dirBoss->dir);
                //vBoss->vel = {300, 300};
                hurtBoxBoss->invincible = true;
                //LOGINFO("%f", dash);
                dash = 0;
            }
        }else if(state == SMASHING){
            if(shoot > shootCd){
                Box2DCollider playerCollider =  calculateCollider(tPlayer, boxPlayer->offset, boxPlayer->size);
                glm::vec2 centerPlayer = getBoxCenter(&playerCollider);
                Box2DCollider bossCollider =  calculateCollider(tBoss, hurtBoxBoss->offset, hurtBoxBoss->size);
                glm::vec2 centerBoss= getBoxCenter(&bossCollider);
                glm::vec2 dir = centerPlayer - centerBoss;
                dir = glm::normalize(dir);
                glm::vec3 pos = glm::vec3((centerBoss + (dir * 25.0f)), tBoss->position.z);
                createProjectile(ecs, engine, pos, dir);
                dirBoss->dir = {0,0};
                currentState = SMASHING;
                shoot = 0;
            }
        }else if (state == STUNNED){
            currentState = STUNNED;
            continue;
        }
        dash += dt;
        shoot += dt;
    }
}

//void bossActiveHurtBoxSystem(Ecs* ecs){
//    //auto bosses = view(ecs, Box2DCollider, HurtBox, BossTag, DirectionComponent);
//    //NOTE: i am sure it's only one right now
//    //for(Entity b : bosses){
//    //    HurtBox* hurtBox = getComponent(ecs, b, HurtBox);
//    //    Box2DCollider* box = getComponent(ecs, b, Box2DCollider);
//    //    DirectionComponent* dir = getComponent(ecs, b, DirectionComponent);
//    //    //if(!hurtBox->invincible){
//    //    //    dir->dir = {0,0};
//    //    //}
//    //}
//}

Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera, glm::vec3 pos){
    Entity boss = createEntity(ecs);
    SpriteComponent sprite = {
        //.texture = getTexture(engine->textureManager, "default"),
        .texture = getTexture(engine->textureManager, "default"),
        .index = {0,0},
        .size = {50, 50},
        .ySort = true,
        .layer = 1.0f
    };
    TransformComponent transform = {    
        .position = {(camera.width / 2) - (sprite.size.x / 2), (camera.height - (camera.height / 3)) - sprite.size.y , 0.0f},
        //.position = pos,
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };
    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .active = true, .offset = {0,0}, .size = sprite.size};
    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size = sprite.size};
    sprite.color = {0,0,1,1};
    BossTag bossTag = {};
    VelocityComponent velocity = {.vel = {300, 300}};
    DirectionComponent direction = {.dir = {0, 0}};
    HitBox hitbox = {.dmg = 1, .offset = collider.offset, .size = collider.size};
    HurtBox hurtbox = {.health = 3, .invincible = true, .offset = {collider.size.x / 2 - 10/2, collider.size.y /2 - 10/2}, .size = {10,10}};
    //hurtBoxCollider.active = false;

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