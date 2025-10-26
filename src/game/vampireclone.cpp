#include "vampireclone.hpp"

#include "components.hpp"

ECS_DECLARE_COMPONENT(ExperienceComponent)

#define MAX_ENEMY_COUNT 300
#define GOBLIN_SPAWN 2
static float spawnTime = 0.01f;
static float orderDuration = 15.0f; 
static uint8_t orderNumber = 1;
static float elapsedTime = 0;
static float orderElapsedTime = 0.0f;

void systemSpawnEnemies(Ecs* ecs, float dt){
    EntityArray players = view(ecs, ECS_TYPE(PlayerTag));
    EntityArray enemies = view(ecs, ECS_TYPE(EnemyTag));

    orderElapsedTime += dt;

    if(orderElapsedTime > orderDuration){
        spawnTime -= spawnTime * 0.5;
        orderElapsedTime = 0;
        orderNumber++;
    }

    //for(Entity player : players){
    for(size_t i = 0; i < players.count; i++){
        Entity player = players.entities[i];
        TransformComponent* transform = (TransformComponent*)getComponent(ecs, player, TransformComponent);
        if(elapsedTime > spawnTime && enemies.count <= MAX_ENEMY_COUNT){
            spawnEnemy(ecs, transform);
            elapsedTime = 0;
        }
    }
    elapsedTime += dt;
}

void systemUpdateEnemyDirection(Ecs* ecs){
    EntityArray players = view(ecs, ECS_TYPE(PlayerTag));
    EntityArray enemies = view(ecs, ECS_TYPE(EnemyTag), ECS_TYPE(DirectionComponent));

    //for(Entity enemy : enemies){
    for(size_t i = 0; i < enemies.count; i++){
        Entity enemy = enemies.entities[i];
        //TransformComponent* enemyTransform = getComponent(ecs, enemy, TransformComponent);
        DirectionComponent* enemyDirection = (DirectionComponent*)getComponent(ecs, enemy, DirectionComponent);
        HitBox* enemyHitbox = (HitBox*)getComponent(ecs, enemy, HitBox);
        //NOTE: we know the player is only 1
        //TransformComponent* playerTransform = getComponent(ecs, players[0], TransformComponent);
        HurtBox* playerHurtBox = (HurtBox*)getComponent(ecs, players.entities[0], HurtBox);
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
    EntityArray enemies = view(ecs, ECS_TYPE(EnemyTag), ECS_TYPE(HitBox));
    EntityArray player = view(ecs, ECS_TYPE(PlayerTag), ECS_TYPE(HurtBox));

    //for(Entity entityA : enemies){
    for(size_t i = 0; i < enemies.count; i++){
        Entity entityA = enemies.entities[i];
        HitBox* boxAent= (HitBox*)getComponent(ecs, entityA, HitBox);
        //for(Entity entityB : player){
        for(size_t j = 0; j < player.count; j++){
            Entity entityB = player.entities[j];
            HurtBox* boxBent = (HurtBox*)getComponent(ecs, entityB, HurtBox);
            if(beginCollision(entityA , entityB) && !boxBent->invincible){
                //boxBent->health -= boxAent->dmg;
                LOGINFO("%f", boxBent->health);
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

    Box2DCollider box = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size = {16,16}, .isTrigger = true};
    pushComponent(ecs, experience, Box2DCollider, &box);
    ExperienceComponent exp = {.xpDrop = 10.0f};
    pushComponent(ecs, experience, ExperienceComponent, &exp);
    //EnemyTag enemyTag;
    //pushComponent(ecs, experience, EnemyTag, &enemyTag);

    DirectionComponent dir = {};
    dir.dir = {0,0};
    pushComponent(ecs, experience, DirectionComponent, &dir);

    VelocityComponent vel = {};
    vel.vel = {200, 200};
    pushComponent(ecs, experience, VelocityComponent, &vel);
}

void spawnSlime(Ecs* ecs, const TransformComponent* playerTransform){
    //srand(time(NULL));
    Entity enemy = createEntity(ecs);
    int radius = 100;
    int outerRadius = 500;
    TransformComponent transform = *playerTransform;
    int resultX = (rand() % (uint32_t)(outerRadius));
    int resultY = (rand() % (uint32_t)(outerRadius));
    int directionX = (rand() % 2) == 0 ? 1 : -1;
    int directionY = (rand() % 2) == 0 ? 1 : -1;
    resultX = (radius + resultX) * directionX;
    resultY = (radius + resultY) * directionY;
    transform.position += glm::vec3(resultX, resultY, 0.0f);
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
    vel.vel = {30, 30};
    pushComponent(ecs, enemy, VelocityComponent, &vel);

    EnemyTag enemyTag;
    pushComponent(ecs, enemy, EnemyTag, &enemyTag);

    HurtBox hurtbox = {.health = 5, .invincible = false, .offset = {24,25}, .size = {16,16}};
    pushComponent(ecs, enemy, HurtBox, &hurtbox);

    HitBox hitbox = {.dmg = 1, .offset = {27,28}, .size = {10,10}};
    pushComponent(ecs, enemy, HitBox, &hitbox);

    Box2DCollider box = {.offset = {27,28}, .size = {10,10}};
    pushComponent(ecs, enemy, Box2DCollider, &box);

    registryAnimation("slime-jump", 8, 1, true);
    AnimationComponent anim = {};
    strncpy(anim.animationId, "slime-jump", sizeof(anim.animationId));
    pushComponent(ecs, enemy, AnimationComponent, &anim);
}

void spawnGoblins(Ecs* ecs, const TransformComponent* playerTransform){
    //srand(time(NULL));
    Entity enemy = createEntity(ecs);
    int radius = 100;
    int outerRadius = 500;
    TransformComponent transform = *playerTransform;
    int resultX = (rand() % (uint32_t)(outerRadius));
    int resultY = (rand() % (uint32_t)(outerRadius));
    int directionX = (rand() % 2) == 0 ? 1 : -1;
    int directionY = (rand() % 2) == 0 ? 1 : -1;
    resultX = (radius + resultX) * directionX;
    resultY = (radius + resultY) * directionY;
    transform.position += glm::vec3(resultX, resultY, 0.0f);
    pushComponent(ecs, enemy, TransformComponent, &transform);

    SpriteComponent sprite = {
        .texture = getTexture("gobu walk"),
        .index = {0,0},
        .size = {32, 32},
        .ySort = true,
        .layer = 1.0f
    };
    pushComponent(ecs, enemy, SpriteComponent, &sprite);

    DirectionComponent dir = {};
    dir.dir = {playerTransform->position.x - transform.position.x, playerTransform->position.y - transform.position.y};
    dir.dir = glm::normalize(dir.dir);
    pushComponent(ecs, enemy, DirectionComponent, &dir);

    VelocityComponent vel = {};
    vel.vel = {10, 10};
    pushComponent(ecs, enemy, VelocityComponent, &vel);

    EnemyTag enemyTag;
    pushComponent(ecs, enemy, EnemyTag, &enemyTag);

    HurtBox hurtbox = {.health = 10, .invincible = false, .offset = {5,2}, .size = {20,20}};
    pushComponent(ecs, enemy, HurtBox, &hurtbox);

    HitBox hitbox = {.dmg = 1, .offset = {10,5}, .size = {10,15}};
    pushComponent(ecs, enemy, HitBox, &hitbox);

    Box2DCollider box = {.offset = {10,5}, .size = {10,15}};
    pushComponent(ecs, enemy, Box2DCollider, &box);

    registryAnimation("gobu-walk", 6, 0, true);
    AnimationComponent anim = {};
    strncpy(anim.animationId, "gobu-walk", sizeof(anim.animationId));
    pushComponent(ecs, enemy, AnimationComponent, &anim);
}

void spawnEnemy(Ecs* ecs, const TransformComponent* playerTransform){
    if(orderNumber < GOBLIN_SPAWN){
        spawnSlime(ecs, playerTransform);
    }else{
        int choice = rand() % 2;
        if(choice == 0){
            spawnSlime(ecs, playerTransform);
        }else{
            spawnGoblins(ecs, playerTransform);
        }
    }
}

void deathEnemySystem(Ecs* ecs){
    EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(HurtBox), ECS_TYPE(EnemyTag));
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        HurtBox* hurtbox = (HurtBox*)getComponent(ecs, e, HurtBox);
        glm::vec2 position = hurtbox->relativePosition;
        //TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        if(hasComponent(ecs, e, PlayerTag)) continue;
        if(hurtbox->health <= 0){
            removeEntity(ecs, e);
            spawnExperience(ecs, position);
        }
    }
}

void levelUp(GameState* gameState, ExperienceComponent* playerXp){
    float fixedXp = 100.0f;
    if(playerXp->currentXp >= playerXp->maxXp){
        playerXp->currentLevel += 1;
        playerXp->maxXp += (float)(fixedXp * playerXp->currentLevel);
        playerXp->currentXp = 0;
        gameState->gameLevels = GameLevels::SELECT_CARD;
    }
    LOGINFO("level: %d | [%f / %f]", playerXp->currentLevel, playerXp->currentXp, playerXp->maxXp);
}

void gatherExperienceSystem(Ecs* ecs, GameState* gameState){
    float radius = 50.0f;
    EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(ExperienceComponent));
    EntityArray players = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(ExperienceComponent), ECS_TYPE(PlayerTag));
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        ExperienceComponent* enemyXp = (ExperienceComponent*)getComponent(ecs, e, ExperienceComponent);
        //TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        Box2DCollider* enemyBox = (Box2DCollider*)getComponent(ecs, e, Box2DCollider);
        DirectionComponent* xpDir = (DirectionComponent*)getComponent(ecs, e, DirectionComponent);

        //TransformComponent* playerTransform = getComponent(ecs, players[0], TransformComponent);
        ExperienceComponent* playerXp = (ExperienceComponent*)getComponent(ecs, players.entities[0], ExperienceComponent);
        Box2DCollider* playerBox = (Box2DCollider*)getComponent(ecs, players.entities[0], Box2DCollider);

        glm::vec2 enemyCenter = getBoxCenter(&enemyBox->relativePosition, &enemyBox->size);
        glm::vec2 playerCenter = getBoxCenter(&playerBox->relativePosition, &playerBox->size);
        if(glm::length(enemyCenter - playerCenter) <= radius){
            xpDir->dir = playerCenter - enemyCenter;
            xpDir->dir = glm::normalize(xpDir->dir);
        }else{
            xpDir->dir = {0,0};
        }
        if(isColliding(e, players.entities[0])){
            playerXp->currentXp += enemyXp->xpDrop;
            levelUp(gameState, playerXp);
            removeEntity(ecs, e);
        }
    }
}


// ------------------------------------------------------- UI CODE ---------------------------------------------------------

void renderPowerUpCards(){
    static bool newButton = false;

    if(UiButton("ciao", {0,0}, {30,30}, {0,0})){
        LOGINFO("CIAO");
        newButton = true;
    }
    if(UiButton("ciao2", {200,240}, {100,40}, {0,0})){
        LOGINFO("CIAO2");
    }

    if(newButton){
        if(UiButton("ciao3", {40,40}, {20,20}, {0,0})){
            LOGINFO("CIAO3");
        }
    }
}