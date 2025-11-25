#include "vampireclone.hpp"

#include "components.hpp"

ECS_DECLARE_COMPONENT(ExperienceComponent)

#define MAX_ENEMY_COUNT 30000
#define GOBLIN_SPAWN 10
static float spawnTime = 0.01f;
static float orderDuration = 60.0f; 
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

    for(size_t i = 0; i < players.count; i++){
        Entity player = players.entities[i];
        TransformComponent* transform = (TransformComponent*)getComponent(ecs, player, TransformComponent);
        if(elapsedTime > spawnTime && enemies.count < MAX_ENEMY_COUNT){
            //for(int i = 0; i < 1000; i++){
                spawnEnemy(ecs, transform);
            //}
            elapsedTime = 0;
        }
    }
    elapsedTime += dt;
}

void systemUpdateEnemyDirection(Ecs* ecs){
    EntityArray players = view(ecs, ECS_TYPE(PlayerTag));
    EntityArray enemies = view(ecs, ECS_TYPE(EnemyTag), ECS_TYPE(DirectionComponent));

    for(size_t i = 0; i < enemies.count; i++){
        Entity enemy = enemies.entities[i];
        DirectionComponent* enemyDirection = (DirectionComponent*)getComponent(ecs, enemy, DirectionComponent);
        glm::vec3 enemyPosition = getComponent(ecs, enemy, TransformComponent)->position;
        glm::vec3 playerPosition = getComponent(ecs, players.entities[0], TransformComponent)->position;

        enemyDirection->dir = glm::vec2(playerPosition.x - enemyPosition.x, playerPosition.y - enemyPosition.y);
        if(glm::length(enemyDirection->dir) != 0){
            enemyDirection->dir = glm::normalize(enemyDirection->dir);
        }
    }
}

void spawnExperience(Ecs* ecs, glm::vec3 position){
    Entity experience = createEntity(ecs);
    TransformComponent transform;
    transform.position = position; 
    transform.scale = {1,1,1};
    transform.rotation = {0,0,0};
    pushComponent(ecs, experience, TransformComponent, &transform);

    SpriteComponent sprite = {
        .texture = getTextureByName("default"),
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f,
        .visible = true
    };
    pushComponent(ecs, experience, SpriteComponent, &sprite);

    ExperienceDrop exp = {.xpDrop = 30.0f};
    pushComponent(ecs, experience, ExperienceDrop, &exp);

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
    int outerRadius = 5000;
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
        .texture = getTextureByName("Slime_Green"),
        .sourceRect = {.pos = {0,0}, .size = {64, 64}},
        .size = {64, 64},
        .ySort = true,
        .layer = 1.0f,
        .visible = true
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

    HealthComponent health = {.hp = 5};
    pushComponent(ecs, enemy, HealthComponent, &health);

    DamageComponent damage = {.dmg = 1};
    pushComponent(ecs, enemy, DamageComponent, &damage);

    Box2DCollider box = {.offset = {0,0}, .size = {10,10}};
    pushComponent(ecs, enemy, Box2DCollider, &box);

    registryAnimation("slime-jump", 8, (uint16_t)1, {64, 64}, true);
    AnimationComponent anim = {};
    strncpy(anim.animationId, "slime-jump", sizeof(anim.animationId));
    pushComponent(ecs, enemy, AnimationComponent, &anim);


    Entity hitbox = createEntity(ecs);
    Box2DCollider hitboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size {16,16}, .isTrigger = true};
    pushComponent(ecs, hitbox, Box2DCollider, &hitboxCollider);
    TransformComponent hitboxTransform = transform;
    pushComponent(ecs, hitbox, TransformComponent, &hitboxTransform);
    Parent parent = {.entity = enemy};
    pushComponent(ecs, hitbox, Parent, &parent);
    HitboxTag hitboxTag = {};
    pushComponent(ecs, hitbox, HitboxTag, &hitboxTag);

    Entity hurtbox = createEntity(ecs);
    Box2DCollider hurtboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size {16,16}, .isTrigger = true};
    pushComponent(ecs, hurtbox, Box2DCollider, &hurtboxCollider);
    TransformComponent hurtboxTransform = transform;
    pushComponent(ecs, hurtbox, TransformComponent, &hurtboxTransform);
    pushComponent(ecs, hurtbox, Parent, &parent);
    HurtboxTag hurtboxTag = {};
    pushComponent(ecs, hurtbox, HurtboxTag, &hurtboxTag);
}

void spawnGoblins(Ecs* ecs, const TransformComponent* playerTransform){
    //srand(time(NULL));
    Entity enemy = createEntity(ecs);
    int radius = 100;
    int outerRadius = 100;
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
        .texture = getTextureByName("gobu walk"),
        .sourceRect = {.pos = {0,0}, .size = {32, 32}},
        .size = {32, 32},
        .ySort = true,
        .layer = 1.0f,
        .visible = true
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

    HealthComponent health = {.hp = 10};
    pushComponent(ecs, enemy, HealthComponent, &health);

    DamageComponent damage = {.dmg = 2};
    pushComponent(ecs, enemy, DamageComponent, &damage);

    Box2DCollider box = {.offset = {0,0}, .size = {10,15}};
    pushComponent(ecs, enemy, Box2DCollider, &box);

    registryAnimation("gobu-walk", 6, (uint16_t)0, {32, 32}, true);
    AnimationComponent anim = {};
    strncpy(anim.animationId, "gobu-walk", sizeof(anim.animationId));
    pushComponent(ecs, enemy, AnimationComponent, &anim);

    Entity hitbox = createEntity(ecs);
    Box2DCollider hitboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size {16,16}, .isTrigger = true};
    pushComponent(ecs, hitbox, Box2DCollider, &hitboxCollider);
    TransformComponent hitboxTransform = transform;
    pushComponent(ecs, hitbox, TransformComponent, &hitboxTransform);
    Parent parent = {.entity = enemy};
    pushComponent(ecs, hitbox, Parent, &parent);
    HitboxTag hitboxTag = {};
    pushComponent(ecs, hitbox, HitboxTag, &hitboxTag);

    Entity hurtbox = createEntity(ecs);
    Box2DCollider hurtboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {0,0}, .size {16,16}, .isTrigger = true};
    pushComponent(ecs, hurtbox, Box2DCollider, &hurtboxCollider);
    TransformComponent hurtboxTransform = transform;
    pushComponent(ecs, hurtbox, TransformComponent, &hurtboxTransform);
    pushComponent(ecs, hurtbox, Parent, &parent);
    HurtboxTag hurtboxTag = {};
    pushComponent(ecs, hurtbox, HurtboxTag, &hurtboxTag);
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

void levelUp(GameState* gameState, ExperienceComponent* playerXp){
    float fixedXp = 10.0f;
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
    EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(ExperienceDrop));
    EntityArray players = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(ExperienceComponent), ECS_TYPE(PlayerTag));
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        ExperienceDrop* enemyXp = (ExperienceDrop*)getComponent(ecs, e, ExperienceDrop);
        TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        DirectionComponent* xpDir = (DirectionComponent*)getComponent(ecs, e, DirectionComponent);

        TransformComponent* playerTransform = getComponent(ecs, players.entities[0], TransformComponent);
        ExperienceComponent* playerXp = (ExperienceComponent*)getComponent(ecs, players.entities[0], ExperienceComponent);
        if(glm::length(transform->position - playerTransform->position) <= radius){
            xpDir->dir = playerTransform->position - transform->position;
            xpDir->dir = glm::normalize(xpDir->dir);
        }else{
            xpDir->dir = {0,0};
        }
        if(glm::length(transform->position - playerTransform->position) <= 10){ //10 is a threashold, need to be setted better
            playerXp->currentXp += enemyXp->xpDrop;
            levelUp(gameState, playerXp);
            removeEntity(ecs, e);
        }
    }
}