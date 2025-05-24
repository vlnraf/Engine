#include "vampireclone.hpp"

void systemSpawnEnemies(Ecs* ecs, OrtographicCamera* camera, float spawnTime, float dt){
    std::vector<Entity> players = view(ecs, PlayerTag);

    static float elapsedTime = 0;

    for(Entity player : players){
        TransformComponent* transform = getComponent(ecs, player, TransformComponent);
        if(elapsedTime > spawnTime){
            spawnEnemy(ecs, transform);
            elapsedTime = 0;
        }
    }
    elapsedTime += dt;
}

void systemUpdateEnemyDirection(Ecs* ecs){
    std::vector<Entity> players = view(ecs, PlayerTag);
    std::vector<Entity> enemies = view(ecs, EnemyTag, DirectionComponent);

    for(Entity enemy : enemies){
        TransformComponent* enemyTransform = getComponent(ecs, enemy, TransformComponent);
        DirectionComponent* enemyDirection = getComponent(ecs, enemy, DirectionComponent);
        HitBox* enemyHitbox = getComponent(ecs, enemy, HitBox);
        //NOTE: we know the player is only 1
        TransformComponent* playerTransform = getComponent(ecs, players[0], TransformComponent);
        HurtBox* playerHurtBox = getComponent(ecs, players[0], HurtBox);
        if(enemyHitbox && playerHurtBox){
            glm::vec2 enemyCenter = getBoxCenter(&enemyHitbox->relativePosition, &enemyHitbox->size);
            glm::vec2 playerCenter = getBoxCenter(&playerHurtBox->relativePosition, &playerHurtBox->size);
            enemyDirection->dir = playerCenter - enemyCenter;
        }
        //enemyDirection->dir = {playerTransform->position.x - enemyTransform->position.x, playerTransform->position.y - enemyTransform->position.y};
        if(glm::length(enemyDirection->dir) != 0){
            enemyDirection->dir = glm::normalize(enemyDirection->dir);
        }
    }
}

void systemEnemyHitPlayer(Ecs* ecs){
    auto enemies = view(ecs, EnemyTag, HitBox);
    auto player = view(ecs, PlayerTag, HurtBox);

    for(Entity entityA : enemies){
        HitBox* boxAent= getComponent(ecs, entityA, HitBox);
        for(Entity entityB : player){
            HurtBox* boxBent = getComponent(ecs, entityB, HurtBox);
            if(beginCollision(entityA , entityB) && !boxBent->invincible){
                boxBent->health -= boxAent->dmg;
                LOGINFO("%d", boxBent->health);
                break;
            }
        }
    }
}

void spawnExperience(Ecs* ecs, glm::vec2 position){
    Entity experience = createEntity(ecs);
    TransformComponent transform;
    transform.position = glm::vec3(position, 0.0f); 
    transform.scale = {1,1,1};
    transform.rotation = {0,0,0};
    pushComponent(ecs, experience, TransformComponent, &transform);

    SpriteComponent sprite = {
        .texture = getTexture("default"),
        .index = {0,0},
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f
    };
    pushComponent(ecs, experience, SpriteComponent, &sprite);

    Box2DCollider box = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {16,16}, .isTrigger = true};
    pushComponent(ecs, experience, Box2DCollider, &box);
    ExperienceComponent exp = {.xpDrop = 10.0f};
    pushComponent(ecs, experience, ExperienceComponent, &exp);
    EnemyTag enemyTag;
    pushComponent(ecs, experience, EnemyTag, &enemyTag);

    DirectionComponent dir = {};
    dir.dir = {0,0};
    pushComponent(ecs, experience, DirectionComponent, &dir);

    VelocityComponent vel = {};
    vel.vel = {200, 200};
    pushComponent(ecs, experience, VelocityComponent, &vel);
}

void spawnEnemy(Ecs* ecs, const TransformComponent* playerTransform){
    //srand(time(NULL));
    Entity enemy = createEntity(ecs);
    int radius = 1000;
    TransformComponent transform = *playerTransform;
    transform.position -= glm::vec3((float)(rand() % (uint32_t)(radius)) - (radius/2) , (float)(rand() % (uint32_t)(radius)) - (radius/2), 0.0f);
    pushComponent(ecs, enemy, TransformComponent, &transform);

    SpriteComponent sprite = {
        .texture = getTexture("Slime_Green"),
        .index = {0,0},
        .size = {64, 64},
        .ySort = true,
        .layer = 1.0f
    };
    pushComponent(ecs, enemy, SpriteComponent, &sprite);

    DirectionComponent dir = {};
    dir.dir = {playerTransform->position.x - transform.position.x, playerTransform->position.y - transform.position.y};
    dir.dir = glm::normalize(dir.dir);
    pushComponent(ecs, enemy, DirectionComponent, &dir);

    VelocityComponent vel = {};
    vel.vel = {50, 50};
    pushComponent(ecs, enemy, VelocityComponent, &vel);

    EnemyTag enemyTag;
    pushComponent(ecs, enemy, EnemyTag, &enemyTag);

    HurtBox hurtbox = {.health = 5, .invincible = false, .offset = {24,25}, .size = {16,16}};
    pushComponent(ecs, enemy, HurtBox, &hurtbox);

    HitBox hitbox = {.dmg = 1, .offset = {27,28}, .size = {10,10}};
    pushComponent(ecs, enemy, HitBox, &hitbox);

    registryAnimation("slime-jump", 8, 1, true);
    AnimationComponent anim = {};
    strncpy(anim.animationId, "slime-jump", sizeof(anim.animationId));
    pushComponent(ecs, enemy, AnimationComponent, &anim);
}

void deathEnemySystem(Ecs* ecs){
    auto entities = view(ecs, TransformComponent, HurtBox, EnemyTag);
    for(Entity e : entities){
        HurtBox* hurtbox = getComponent(ecs, e, HurtBox);
        TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        if(hasComponent(ecs, e, PlayerTag)) continue;
        if(hurtbox->health <= 0){
            spawnExperience(ecs, hurtbox->relativePosition);
            removeEntity(ecs, e);
        }
    }
}

void gatherExperienceSystem(Ecs* ecs, GameState* gameState){
    float radius = 50.0f;
    float fixedXp = 100.0f;
    auto entities = view(ecs, TransformComponent, ExperienceComponent, EnemyTag);
    auto players = view(ecs, TransformComponent, ExperienceComponent, PlayerTag);
    for(Entity e : entities){
        ExperienceComponent* enemyXp = getComponent(ecs, e, ExperienceComponent);
        TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        Box2DCollider* enemyBox = getComponent(ecs, e, Box2DCollider);
        DirectionComponent* xpDir = getComponent(ecs, e, DirectionComponent);

        TransformComponent* playerTransform = getComponent(ecs, players[0], TransformComponent);
        ExperienceComponent* playerXp = getComponent(ecs, players[0], ExperienceComponent);
        Box2DCollider* playerBox = getComponent(ecs, players[0], Box2DCollider);

        glm::vec2 enemyCenter = getBoxCenter(&enemyBox->relativePosition, &enemyBox->size);
        glm::vec2 playerCenter = getBoxCenter(&playerBox->relativePosition, &playerBox->size);
        if(glm::length(enemyCenter - playerCenter) <= radius){
            xpDir->dir = playerCenter - enemyCenter;
            xpDir->dir = glm::normalize(xpDir->dir);
        }else{
            xpDir->dir = {0,0};
        }
        if(beginCollision(e, players[0])){
            playerXp->currentXp += enemyXp->xpDrop;
            if(playerXp->currentXp >= playerXp->maxXp){
                playerXp->currentLevel += 1;
                playerXp->maxXp += (float)(fixedXp * playerXp->currentLevel);
                playerXp->currentXp = 0;
                gameState->pause = true;
            }
            LOGINFO("level: %d | [%f / %f]", playerXp->currentLevel, playerXp->currentXp, playerXp->maxXp);
            removeEntity(ecs, e);
        }
    }
}


// ------------------------------------------------------- UI CODE ---------------------------------------------------------

static uint32_t active = false;
static uint32_t hot = false;
static uint32_t id = 0;

bool UIButton(const char* fmt, glm::vec2 pos, glm::vec2 size, glm::vec3 rotation, glm::vec4 color){

    glm::vec2 mousePos = getMousePos();
    LOGINFO("xpos %f, ypos %f", (float)mousePos.x, (float)mousePos.y);
    
    renderDrawFilledRect(pos, size, rotation, color);
    renderDrawTextUI(fmt, pos.x, pos.y, 1.0f);
    return false;
}

void renderPowerUpCards(EngineState* engine, GameState* gameState){
    beginUIScene({0,0}, {engine->windowWidth, engine->windowHeight});
        UIButton("ciao", {200,200}, {100,40}, {0,0,0}, {0.0f,0.0f,0.0f,0.70f});
        UIButton("ciao", {350,200}, {100,40}, {0,0,0}, {0.0f,0.0f,0.0f,0.70f});
    endUIScene();
}