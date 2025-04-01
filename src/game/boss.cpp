#include "boss.hpp"
#include "gamekit/colliders.hpp"
#include "spike.hpp"
#include "telegraphattack.hpp"

#define radice2 1.41421356f

enum BossState{
    IDLE,
    DASHING,
    SMASHING,
    STUNNED
};

BossState currentState = IDLE;
BossState nextState = IDLE;

Texture* hurtTexture;
Texture* idleTexture;

//NOTE: probably this system is not good design wise
void systemRespondBossHitStaticEntity(Ecs* ecs, const float dt){
    auto entitiesA = view(ecs, DirectionComponent, HurtBox, HitBox, BossTag);
    auto entitiesB = view(ecs, Box2DCollider);

    for(Entity entityA : entitiesA){
        HurtBox* boxAentHurt= getComponent(ecs, entityA, HurtBox);
        DirectionComponent* dirA = getComponent(ecs, entityA, DirectionComponent);
        for(Entity entityB : entitiesB){
            if(entityA == entityB) continue; //skip self collision

            Box2DCollider* boxBent = getComponent(ecs, entityB, Box2DCollider);
            if(beginCollision(entityA, entityB) && boxBent->type == Box2DCollider::STATIC){
                boxAentHurt->invincible = false;
                dirA->dir = {0,0};
                currentState = STUNNED;
            }
        }
    }
}

void changeBossTextureSystem(Ecs* ecs){
    auto bosses = view(ecs, BossTag, SpriteComponent, HurtBox);

    for(Entity e : bosses){
        SpriteComponent* sprite = getComponent(ecs, e, SpriteComponent);
        HurtBox* bossHurt = getComponent(ecs, e, HurtBox);

        if(bossHurt->invincible){
            sprite->texture = idleTexture;
        }else{
            sprite->texture = hurtTexture;
        }
    }
}

void bossAiSystem(Ecs* ecs, EngineState* engine, OrtographicCamera camera, float dt){
    auto bosses = view(ecs, BossTag, HurtBox, DirectionComponent, TransformComponent, VelocityComponent);
    auto players = view(ecs, PlayerTag);
    //NOTE: i am sure it's only one right now
    Entity player = players[0];


    for(Entity b : bosses){
        static bool telegraphSpawned = false;

        static float dash = 0;
        static bool dashUsed = false;
        float dashDur = 1;
        float dashCd = 1.0;
        static float shoot = 0;
        float shootCd = 0.5;
        static int wave = 1;
        int waves = 10;
        static float stun = 0;
        float stunCd = 0.1;
        DirectionComponent* dirBoss = getComponent(ecs, b, DirectionComponent);
        TransformComponent* tBoss = getComponent(ecs, b, TransformComponent);
        //VelocityComponent* vBoss = getComponent(ecs, b, VelocityComponent);
        HurtBox* hurtBoxBoss = getComponent(ecs, b, HurtBox);

        Box2DCollider* boxPlayer = getComponent(ecs, player, Box2DCollider);
        TransformComponent* tPlayer = getComponent(ecs, player, TransformComponent);
        //BossState state = (BossState)((rand() % 2) + 1); //SKIPPING IDLE AND STUNNED right now
        //srand(time(NULL));
        if(nextState == IDLE){
            //nextState = (BossState)((rand() % 2) + 1); //SKIPPING IDLE AND STUNNED right now
            nextState = DASHING;
            //continue;
        }else if(nextState == DASHING){
            if(dash > dashDur && dashUsed){
                dash = 0;
                dashUsed = false;
                nextState = SMASHING;
            }
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
                dashUsed = true;
                dash = 0;
            }
            dash += dt;
        }else if(nextState == SMASHING){
            Box2DCollider playerCollider =  calculateCollider(tPlayer, boxPlayer->offset, boxPlayer->size);
            glm::vec2 centerPlayer = getBoxCenter(&playerCollider);
            Box2DCollider bossCollider =  calculateCollider(tBoss, hurtBoxBoss->offset, hurtBoxBoss->size);
            glm::vec2 centerBoss= getBoxCenter(&bossCollider);
            //glm::vec2 dir = centerPlayer - centerBoss;
            //dir = glm::normalize(dir);
            dirBoss->dir = {0,0};
            glm::vec2 directions[8] = {{0,1}, {radice2 / 2, radice2 / 2}, {1,0}, {radice2 / 2, (-radice2) / 2}, {0, -1}, {(-radice2) / 2, (-radice2) / 2}, {-1, 0}, {(-radice2) / 2, radice2 / 2}};
            //if(shoot < shootCd){
                if(!telegraphSpawned){
                    for(int i = 0; i < 8; i++){
                        glm::vec3 pos = glm::vec3(centerBoss + ((directions[i] * 20.0f) * (float)wave), tBoss->position.z);
                        //renderDrawFilledRect(engine->renderer, camera, {pos.x - (15/2), pos.y - (15/2), pos.z}, {15, 15}, {0,0,0}, {1,0,0,0.5});
                        createTelegraphAttack(ecs, engine, camera, pos);
                    }
                    telegraphSpawned = true;
                }
                nextState = SMASHING;
            //}else{
            if(shoot > shootCd){
                //glm::vec3 pos = glm::vec3((centerBoss + (dir * 25.0f)), tBoss->position.z);
                for(int i = 0; i < 8; i++){
                    glm::vec3 pos = glm::vec3(centerBoss + ((directions[i] * 20.0f) * (float)wave), tBoss->position.z);
                    createSpike(ecs, engine, pos);
                }
                dirBoss->dir = {0,0};
                currentState = SMASHING;
                nextState = SMASHING;
                shoot = 0;
                telegraphSpawned = false;
                wave++;
                if(wave > waves){
                    wave = 0;
                    nextState = STUNNED;
                }
            }
            shoot += dt;
        }else if (nextState == STUNNED){
            currentState = STUNNED;
            if(stun > stunCd){
                nextState = DASHING;
                stun = 0;
            }
            stun += dt;
        }
    }
}

Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera, glm::vec3 pos){
    Entity boss = createEntity(ecs);
    idleTexture = getTexture("default");
    hurtTexture = getTexture("Golem-hurt");
    SpriteComponent sprite = {
        .texture = getTexture("default"),
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