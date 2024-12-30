#include <malloc.h>

#include "game.hpp"
#include "gameserializer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


#define ACTIVE_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 255.0f / 255.0f, 255.0f  /255.0f)
#define DEACTIVE_COLLIDER_COLOR glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f)
#define HIT_COLLIDER_COLOR glm::vec4(0 , 255.0f / 255.0f, 0, 255.0f  /255.0f)
#define HURT_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 0, 255.0f / 255.0f)
//#define TRIGGER_COLLIDER_COLOR glm::vec4(0, 255.0f /255.0f, 0, 255.0f / 255.0f)

MyProfiler prof;

void systemRenderSprites(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        if(s->visible){
            renderDrawSprite(renderer, gameState->camera, t->position, t->scale, t->rotation, s);
        }
    }
    PROFILER_END();
}

void systemRenderColliders(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        Box2DCollider* box= (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider b = calculateWorldAABB(t, box);
        if(box->active){
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, ACTIVE_COLLIDER_COLOR, 30);
        }else{
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        }
    }
    PROFILER_END();
}

void systemRenderHitBox(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        HitBox* hitBox= (HitBox*) getComponent(ecs, entity, ECS_HITBOX);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hit = calculateWorldAABB(t, &hitBox->area);
        renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, HIT_COLLIDER_COLOR, 30);
    }
    PROFILER_END();
}

void systemRenderHurtBox(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        HurtBox* hurtBox= (HurtBox*) getComponent(ecs, entity, ECS_HURTBOX);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hurt = calculateWorldAABB(t, &hurtBox->area);
        renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, HURT_COLLIDER_COLOR, 30);
    }
    PROFILER_END();
}

void systemCheckHitBox(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, ECS_HITBOX);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        DebugNameComponent* nameComponentA = (DebugNameComponent*) getComponent(ecs, entityA, ECS_DEBUG_NAME);
        for(Entity entityB : entitiesB){
            if(entityA == entityB) continue; //skip self collision

            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, ECS_HURTBOX);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            DebugNameComponent* nameComponentB = (DebugNameComponent*) getComponent(ecs, entityB, ECS_DEBUG_NAME);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, &boxAent->area); 
            Box2DCollider boxB = calculateWorldAABB(tB, &boxBent->area); 

            if(isColliding(&boxA, &boxB)){
                if(boxAent->hit && !boxAent->alreadyHitted){
                    boxBent->health -= boxAent->dmg;
                    //LOGINFO("entity %s hitted %s, %s health: %d", nameComponentA->name.c_str(), nameComponentB->name.c_str(), nameComponentB->name.c_str(), boxBent->health);
                    boxAent->alreadyHitted = true;
                    boxBent->hitted = true;
                }
                boxAent->discover = true;
            }
        }
    }
}

void systemCollision(GameState* gameState, Ecs* ecs, float dt){
    PROFILER_START();
    std::vector<Entity> dynamicEntities;
    std::vector<Entity> staticEntities;
    std::vector<Entity> colliderEntities = view(ecs, {ECS_2D_BOX_COLLIDER});
    for(Entity e : colliderEntities){
        Box2DCollider* collider = (Box2DCollider*) getComponent(ecs, e, ECS_2D_BOX_COLLIDER);
        if(collider->type == Box2DCollider::STATIC && collider->active){
            staticEntities.push_back(e);
        }else if(collider->type == Box2DCollider::DYNAMIC && collider->active){
            dynamicEntities.push_back(e);
        }
    }
    systemCheckCollisionDynamicDynamic(ecs, dynamicEntities, dynamicEntities, dt);
    systemCheckCollisionDynamicStatic(ecs, dynamicEntities, staticEntities, dt);

    std::vector<Entity> weaponHitBoxes = view(ecs, {ECS_HITBOX, ECS_WEAPON});
    std::vector<Entity> enemyHitBoxes = view(ecs, {ECS_HITBOX, ECS_ENEMY_TAG});
    std::vector<Entity> playerHurtBoxes = view(ecs, {ECS_HURTBOX, ECS_PLAYER_TAG});
    std::vector<Entity> monsterHurtBoxes = view(ecs, {ECS_HURTBOX, ECS_ENEMY_TAG});
    systemCheckHitBox(ecs, weaponHitBoxes, monsterHurtBoxes, dt);
    systemCheckHitBox(ecs, enemyHitBoxes, playerHurtBoxes, dt);
    PROFILER_END();
}

void animationSystem(GameState* gameState, Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    //NOTE: It should not be a system or it runs every frame and so even if the animation
    // is not showing it's been computed
    for(Entity entity : entities){
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        AnimationComponent* component = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        Animation* animation = getAnimation(&gameState->animationManager, component->id.c_str());
        component->frames = animation->frames;
        component->loop = animation->loop;
        component->frameCount += dt;

        if(component->id != component->previousId){ //NOTE: synchronize animation to frame 0 when it changes
            component->currentFrame = 0;
            component->previousId = component->id;
        }

        if(component->loop){
            if(component->frameCount >= animation->frameDuration){
                component->currentFrame = (component->currentFrame + 1) % (animation->frames); // module to loop around
                component->frameCount = 0;
            }
        }else{
            if(component->frameCount >= animation->frameDuration){
                component->currentFrame = component->currentFrame + 1; 
                component->frameCount = 0;
            }
        }
        s->index = animation->indices[component->currentFrame];
    }
    PROFILER_END();
}

void moveSystem(Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
        DirectionComponent* dir = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        transform->position.x += dir->dir.x * vel->vel.x * dt;
        transform->position.y += dir->dir.y * vel->vel.y * dt;
    }
    PROFILER_END();
}

void systemUpdateAttachedEntity(Ecs* ecs, std::vector<ComponentType> types){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        AttachedEntity* attach = (AttachedEntity*) getComponent(ecs, entity, ECS_ATTACHED_ENTITY);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        TransformComponent* transformAttach = (TransformComponent*) getComponent(ecs, attach->entity, ECS_TRANSFORM);
        if(!transform || ! transformAttach){
            PROFILER_END();
            return;
        }
        transform->position.x = transformAttach->position.x + attach->offset.x;
        transform->position.y = transformAttach->position.y + attach->offset.y;
    }
    PROFILER_END();
}

void inputSystemWeapon(GameState* gameState, Ecs* ecs, Input* input, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    //TODO: create a parametrized animation???
    const float speed = 300.0f;
    static float currentFrame = 0;
    float animationDuration = 0.30;
    static bool startAnimation = false;

    static float abilityDt = 0;
    float cooldown = 1;
    static bool inCooldown = false;

    for(Entity entity : entities){
        HitBox* hitBox = (HitBox*) getComponent(ecs, entity, ECS_HITBOX);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //WeaponTag* weapon = (WeaponTag*) getComponent(ecs, entity, ECS_WEAPON);

        if((input->keys[KEYS::Space] || input->gamepad.buttons[GAMEPAD_BUTTON_X]) && (currentFrame < animationDuration) && !startAnimation){
            startAnimation = true;
        }
        if(startAnimation && !inCooldown){
            currentFrame += dt;
            sprite->visible = true;
            transform->rotation.z -= dt * speed;
            if(hitBox->discover){
                hitBox->hit = true;
            }
        }
        if(currentFrame > animationDuration){
            hitBox->hit = false;
            hitBox->alreadyHitted = false;
            transform->rotation.z = 0;
            sprite->visible = false;
            currentFrame = 0;
            startAnimation = false;
            inCooldown = true;
        }
        if(inCooldown){
            abilityDt += dt;
        }
        if(abilityDt > cooldown){
            inCooldown = false;
            abilityDt = 0;
        }
    }

    PROFILER_END();
}

void inputSystem(GameState* gameState, Ecs* ecs, Input* input, std::vector<ComponentType> types, const float dt){
    PROFILER_START();

    float speed = 100.0f;

    float dashCooldown = 1.0;
    float dashActive = 0.2;
    static float dashDt = 0;
    static bool dashAbility = false;
    float speedDash = 300.0f;

    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        VelocityComponent* velocity = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
        AnimationComponent* data = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        HurtBox* hurtBox = (HurtBox*) getComponent(ecs, entity, ECS_HURTBOX);
        direction->dir = {0,0};
        {   //GamePad
            //Animation* anim = getAnimation(&gameState->animationManager, data->id.c_str());//->animations.at("idleRight");
            //data->frameDuration = 1.0f / anim->frames;
            data->id = "idleLeft";
            if(input->gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER]){LOGINFO("Trigger Sinistro");}
            if(input->gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER]){LOGINFO("Trigger Destro");}
            if(fabs(input->gamepad.leftX) > 0.1 || fabs(input->gamepad.leftY) > 0.1){ //threshold because it's never 0.0
                //NOTE: Input entity can have no animation component, error prone
                //TODO: just implement the change of animation logic elsewhere
                //I think the input system is not the system where do this logic
                //And implement direction logic instead of threshold
                velocity->vel.x = speed * fabs(input->gamepad.leftX); //NOTE: aboslute value because then will be multiplied with the direction
                velocity->vel.y = speed * fabs(input->gamepad.leftY); //NOTE: aboslute value because then will be multiplied with the direction
                //LOGINFO("%f / %f", velocity->vel.x, velocity->vel.y);
                if(input->gamepad.leftX < -0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    sprite->flipX = true;
                    data->id = "walkLeft";
                }else if(input->gamepad.leftX > 0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    sprite->flipX = false;
                    data->id = "walkRight";
                }else if(input->gamepad.leftY > 0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    data->id = "walkTop";
                }else if(input->gamepad.leftY < -0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    data->id = "walkBottom";
                }
            }
            //setComponent(ecs, entity, data, ECS_ANIMATION);
        }
        if(input->keys[KEYS::W]){ 
            direction->dir.y = 1;
            velocity->vel.y = speed;
            data->id = "walkTop";
        }
        if(input->keys[KEYS::S]){
            direction->dir.y = -1;
            velocity->vel.y = speed;
            data->id = "walkBottom";
        }
        if(input->keys[KEYS::A]){ 
            direction->dir.x = -1;
            velocity->vel.x = speed;
            sprite->flipX = true;
            data->id = "walkLeft";
        }
        if(input->keys[KEYS::D]){ 
            direction->dir.x = 1;
            velocity->vel.x = speed;
            sprite->flipX = false;
            data->id = "walkRight";
        }

        if(input->gamepad.buttons[GAMEPAD_BUTTON_A]){
            dashAbility = true;
        }
        if(dashAbility){
            VelocityComponent* velT = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
            velT->vel = {speedDash, speedDash};
            dashDt += dt;
        }
        if(dashDt > dashActive){
            VelocityComponent* velT = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
            velT->vel = {speed, speed};
        }
        if(dashDt > dashCooldown){
            dashDt = 0;
            dashAbility = false;
        }
        if(hurtBox->health <= 0){
            LOGINFO("Game Over");
        }

        //NOTE: should i normalize the direction???
        if (direction->dir.x != 0 || direction->dir.y != 0) {
            direction->dir = glm::normalize(direction->dir);
        }
    }
    PROFILER_END();
}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera, std::vector<ComponentType> types){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        TransformComponent* t = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        if(!t){
            PROFILER_END();
            return;
        }
        followTarget(camera, t->position);
    }
    PROFILER_END();
}

void enemyFollowPlayerSystem(Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    //NOTE: If the entity is not in the map enymore do nothing
    for(Entity entity : entities){
        EnemyTag* enemyTag = (EnemyTag*) getComponent(ecs, entity, ECS_ENEMY_TAG);
        Entity toFollow = enemyTag->toFollow;
        TransformComponent* transformP = (TransformComponent*) getComponent(ecs, toFollow, ECS_TRANSFORM);
        Box2DCollider* boxP = (Box2DCollider*) getComponent(ecs, toFollow, ECS_2D_BOX_COLLIDER);
        Box2DCollider boxPlayer = calculateWorldAABB(transformP, boxP);
        if(!transformP){
            PROFILER_END();
            return;
        }

        TransformComponent* t = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        DirectionComponent* dir = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        AnimationComponent* anim = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        Box2DCollider* box = (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        Box2DCollider boxEnemy = calculateWorldAABB(t, box);
        HurtBox* hurtBox = (HurtBox*) getComponent(ecs, entity, ECS_HURTBOX);
        HitBox* hitBox = (HitBox*) getComponent(ecs, entity, ECS_HITBOX);

        if(hurtBox->health <= 0){
            anim->id = "monsterDeath";
        }
        if(std::strcmp(anim->id.c_str(), "monsterDeath") == 0){
            if(anim->currentFrame < anim->frames-1){
                continue;
            }else{
                removeEntity(ecs, entity);
            }
        }
        if(hurtBox->hitted){
            anim->id = "monsterHit";
        }
        if(std::strcmp(anim->id.c_str(), "monsterHit") == 0){
            if(anim->currentFrame < anim->frames-1){
                continue;
            }else{
                hurtBox->hitted = false;
            }
        }

        if(hitBox->discover){
            anim->id = "monsterAttack";
            dir->dir = {0,0};
        }
        if(std::strcmp(anim->id.c_str(), "monsterAttack") == 0){
            if(anim->currentFrame > 9 && anim->currentFrame <= 13){ //9 - 12 frames to deal dmg
                hitBox->hit = true;
            }
            if(anim->currentFrame <= anim->frames){
                continue;
            }else{
                hitBox->hit = false;
                hitBox->discover = false;
                hitBox->alreadyHitted = false;
            }
        }
        anim->id = "monsterWalk";
        dir->dir.x = (boxPlayer.offset.x + boxPlayer.size.x) - (boxEnemy.offset.x);
        dir->dir.y = (boxPlayer.offset.y) - boxEnemy.offset.y;
        if(fabs(dir->dir.x) > 0 && fabs(dir->dir.y) > 0){
            dir->dir = glm::normalize(dir->dir);
        }else{
            dir->dir = {0,0};
        }
    }
    PROFILER_END();
}


GAME_API void gameStart(EngineState* engine){//, GameState* gameState){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        //return nullptr;
        return;
    }
    PROFILER_SAVE("profiler.json");
    LOGINFO("Game Start");

    //GameState* gameState = new GameState();
    PROFILER_START();
    GameState* gameState = new GameState();
    engine->gameState = gameState;
    gameState->ecs = initEcs();
    //TODO: make a resource manager
    //I think this also slow down the boot-up, so we can load textures with another thread
    //Texture* demonSprite = loadTexture("assets/demon.png");
    //Texture* white = getWhiteTexture();
    //Texture* tileSet = loadTexture("assets/sprites/tileset01.png");
    //Texture* idleWalk = loadTexture("assets/idle-walk.png");
    //Texture* treeSprite = loadTexture("assets/sprites/tree.png");
    //Texture* weaponSprite = loadTexture("assets/sprites/wood.png");

    //TODO: make a resource manager
    //I think this also slow down the boot-up, so we can load textures with another thread
    //gameState->textureManager = initTextureManager();
    int demon = loadTextureInManager(engine->textureManager, "assets/demon.png");
    int tileset = loadTextureInManager(engine->textureManager, "assets/sprites/tileset01.png");
    int playerSprite = loadTextureInManager(engine->textureManager, "assets/idle-walk.png");
    int treeSprite = loadTextureInManager(engine->textureManager, "assets/sprites/tree.png");
    int swordSprite = loadTextureInManager(engine->textureManager, "assets/sprites/wood.png");

    TileSet simple = createTileSet(getTexture(engine->textureManager, tileset), 32);

    std::vector<int> tileBg = loadTilemapFromFile("assets/map/map-bg.csv", simple, 30);
    std::vector<int> tileFg = loadTilemapFromFile("assets/map/map-fg.csv", simple, 30);

    gameState->bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    gameState->fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    TransformComponent transform = {};
    transform.position = glm ::vec3(10.0f, 10.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);

    SpriteComponent sprite = {};
    //sprite.texture = getWhiteTexture();

    InputComponent inputC = {};

    VelocityComponent velocity = {.vel = {0, 0}};

    DirectionComponent direction = {.dir = {1,0}};

    gameState->camera = createCamera(glm::vec3(0.0f, 0.0f, 0.0f), 640, 320);


    gameState->animationManager = initAnimationManager();
    {   //Animatioin registry
        registryAnimation(&gameState->animationManager, "idleRight", 4, (uint16_t[]){0,1,2,3}, 0, true);
        registryAnimation(&gameState->animationManager, "idleLeft", 4, (uint16_t[]){0,1,2,3}, 0, true);
        registryAnimation(&gameState->animationManager, "idleBottom", 4, (uint16_t[]){0,1,2,3}, 1, true);
        registryAnimation(&gameState->animationManager, "idleTop", 4, (uint16_t[]){0,1,2,3}, 2, true);
        registryAnimation(&gameState->animationManager, "walkRight", 8, (uint16_t[]){0,1,2,3}, 3, true);
        registryAnimation(&gameState->animationManager, "walkLeft", 8, (uint16_t[]){0,1,2,3}, 3, true);
        registryAnimation(&gameState->animationManager, "walkBottom", 8, (uint16_t[]){0,1,2,3}, 4, true);
        registryAnimation(&gameState->animationManager, "walkTop", 8, (uint16_t[]){0,1,2,3}, 5, true);
    }

    {   //Animatioin Monster
        registryAnimation(&gameState->animationManager, "monsterIdle", 6, 0, true);
        registryAnimation(&gameState->animationManager, "monsterWalk", 12, 1, true);
        registryAnimation(&gameState->animationManager, "monsterAttack", 15, 2, false);
        registryAnimation(&gameState->animationManager, "monsterHit", 5, 3, false);
        registryAnimation(&gameState->animationManager, "monsterDeath", 22, 4, false);
    }

    deserializeGame(engine, gameState, "test");

    //transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    //transform.scale = glm ::vec3(1.0f, 1.0f, 0.0f);
    //transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    //Entity player = createEntity(gameState->ecs, "player", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    ////char* texturePath = "assets/idle-walk.png";
    ////sprite.texture = loadTexture("assets/idle-walk.png");//idleWalk;
    //sprite.texture = getTexture(gameState->textureManager, playerSprite);
    //sprite.textureIndex = playerSprite;
    ////sprite.textureIndex = loadTextureInManager(gameState->textureManager, "assets/idle-walk.png");
    ////sprite.texturePath = "assets/idle-walk.png";
    ////std::strncpy(sprite.texturePath, "assets/idle-walk.png", sizeof(sprite.texturePath));
    //sprite.index = {0,0};
    //sprite.size = {16, 16};
    //sprite.layer = 1.0f;
    //sprite.ySort = true;
    //Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {0, 0}, .size = {16, 5}};



    //AnimationComponent anim = {};
    ////anim.id = "idleRight";

    //PlayerTag playerTag = {};
    //HurtBox hurtBox = {.health=100, .area = {.offset = {4, 0}, .size = {10, 16}}};

    //pushComponent(gameState->ecs, player, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    //pushComponent(gameState->ecs, player, ECS_DIRECTION, &direction, sizeof(DirectionComponent));
    //pushComponent(gameState->ecs, player, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    //pushComponent(gameState->ecs, player, ECS_HURTBOX, &hurtBox, sizeof(HurtBox));
    //pushComponent(gameState->ecs, player, ECS_INPUT, &inputC, sizeof(InputComponent));
    //pushComponent(gameState->ecs, player, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
    //pushComponent(gameState->ecs, player, ECS_ANIMATION, &anim, sizeof(AnimationComponent));
    //pushComponent(gameState->ecs, player, ECS_PLAYER_TAG, &playerTag, sizeof(PlayerTag));


    //AttachedEntity attached = {.entity = player, .offset ={5, 0}};
    //WeaponTag weaponTag = {};
    //transform.scale = glm ::vec3(1.0f, 1.0f, 0.0f);
    //transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    ////sprite.texture = loadTexture("assets/sprites/wood.png");//weaponSprite;
    //sprite.texture = getTexture(gameState->textureManager, swordSprite);
    //sprite.textureIndex = swordSprite;
    ////std::strncpy(sprite.texturePath, "assets/sprites/wood.png", sizeof(sprite.texturePath));
    //sprite.pivot = SpriteComponent::PIVOT_BOT_LEFT;
    //sprite.index = {0,0};
    //sprite.size = {15, 48};
    //sprite.layer = 1.0f;
    //sprite.ySort = true;
    //sprite.visible = false;
    ////collider = {.active = false, .offset = {30, -20}, .size = {20, 50}};//.size = {sprite.size.x * transform.scale.x, sprite.size.y * transform.scale.y}};
    //HitBox hitBox = {.dmg = 10, .area = {.active = false, .offset = {30, -20}, .size = {20, 50}}};
    //Entity weapon = createEntity(gameState->ecs, "weapon", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //pushComponent(gameState->ecs, weapon, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    ////pushComponent(gameState->ecs, weapon, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    //pushComponent(gameState->ecs, weapon, ECS_HITBOX, &hitBox, sizeof(HitBox));
    //pushComponent(gameState->ecs, weapon, ECS_ATTACHED_ENTITY, &attached, sizeof(AttachedEntity));
    //pushComponent(gameState->ecs, weapon, ECS_INPUT, &inputC, sizeof(InputComponent));
    //pushComponent(gameState->ecs, weapon, ECS_WEAPON, &weaponTag, sizeof(WeaponTag));

    //transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    //transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    //transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    //Entity tree = createEntity(gameState->ecs, "tree", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //collider = {.type = Box2DCollider::STATIC, .offset = {20, 0}, .size = {30, 10}};
    //sprite.pivot = SpriteComponent::PIVOT_CENTER;
    ////sprite.texture = loadTexture("assets/sprites/tree.png");//treeSprite;
    //sprite.texture = getTexture(gameState->textureManager, treeSprite);
    //sprite.textureIndex = treeSprite;
    ////std::strncpy(sprite.texturePath, "assets/sprites/tree.png", sizeof(sprite.texturePath));
    //sprite.index = {0,0};
    //sprite.size = {sprite.texture->width, sprite.texture->height};
    //sprite.layer = 1.0f;
    //sprite.ySort = true;
    //sprite.visible = true;
    //pushComponent(gameState->ecs, tree, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    //pushComponent(gameState->ecs, tree, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));

    //srand(time(NULL));

    //for(int i = 0; i < 1; i++){
    //    transform.position = glm::vec3(rand() % uint32_t((gameState->fgMap.width * gameState->fgMap.tileSize) -100) , rand() % uint32_t((gameState->fgMap.height * gameState->fgMap.tileSize) -100), 0.0f);
    //    //transform.scale = glm ::vec3(0.05f, 0.05f , 0.0f);
    //    transform.scale = glm ::vec3(1.0f, 1.0f, 1.0f);
    //    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    //    Entity enemy = createEntity(gameState->ecs, "enemy", ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    //    //sprite.texture = loadTexture("assets/demon.png");//demonSprite;
    //    sprite.textureIndex = demon;
    //    sprite.texture = getTexture(gameState->textureManager, demon);
    //    //std::strncpy(sprite.texturePath, "assets/demon.png", sizeof(sprite.texturePath));
    //    //strcpy(sprite.texturePath, "assets/demon.png");
    //    sprite.index = {0,0};
    //    sprite.size = {288, 160};
    //    sprite.offset = {0, 0};
    //    sprite.ySort = true;
    //    sprite.visible = true;
    //    sprite.layer = 1.0f;
    //    sprite.pivot = SpriteComponent::PIVOT_CENTER;
    //    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {110, 0}, .size = {60, 20}};
    //    HurtBox hurtBox = {.health=100, .area = {.offset = {110, 30}, .size = {60, 40}}};
    //    HitBox hitBox = {.dmg = 50, .area = {.offset = {40, 0}, .size = {70, 40}}};
    //    velocity.vel = {15.0f, 15.0f};
    //    DirectionComponent direction = {.dir = {0,0}};
    //    EnemyTag enemyTag = {.dmg = 10, .toFollow = player};
    //    pushComponent(gameState->ecs, enemy, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    //    pushComponent(gameState->ecs, enemy, ECS_DIRECTION, &direction, sizeof(DirectionComponent));
    //    pushComponent(gameState->ecs, enemy, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
    //    pushComponent(gameState->ecs, enemy, ECS_ENEMY_TAG, &enemyTag, sizeof(EnemyTag));
    //    pushComponent(gameState->ecs, enemy, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    //    pushComponent(gameState->ecs, enemy, ECS_HURTBOX, &hurtBox, sizeof(HurtBox));
    //    pushComponent(gameState->ecs, enemy, ECS_HITBOX, &hitBox, sizeof(HitBox));
    //    pushComponent(gameState->ecs, enemy, ECS_ANIMATION, &anim, sizeof(AnimationComponent));
    //}

    //transform.position = {0,0,0};
    //Entity leftEdge = createEntity(gameState->ecs, "leftEdge", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //collider = {.type = Box2DCollider::STATIC, .offset = {-10, 0}, .size = {gameState->fgMap.tileSize, gameState->fgMap.height * gameState->fgMap.tileSize}};
    //pushComponent(gameState->ecs, leftEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    //Entity rightEdge = createEntity(gameState->ecs, "rightEdge", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //collider = {.type = Box2DCollider::STATIC, .offset = {gameState->fgMap.width * gameState->fgMap.tileSize - 10, 0}, .size = {gameState->fgMap.width, gameState->fgMap.height * gameState->fgMap.tileSize}};
    //pushComponent(gameState->ecs, rightEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    //Entity bottomEdge = createEntity(gameState->ecs, "bottomEdge", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {gameState->fgMap.width * gameState->fgMap.tileSize, gameState->fgMap.tileSize}};
    //pushComponent(gameState->ecs, bottomEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    //Entity topEdge = createEntity(gameState->ecs, "topEdge", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    //collider = {.type = Box2DCollider::STATIC, .offset = {0, gameState->fgMap.height * gameState->fgMap.tileSize - 32}, .size = {gameState->fgMap.width * gameState->fgMap.tileSize, gameState->fgMap.tileSize}};
    //pushComponent(gameState->ecs, topEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    ////removeEntity(gameState->ecs, player);
    PROFILER_END();

    //return gameState;
}

GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt){
    PROFILER_START();
    //if (!gladLoadGL()) {
    //    LOGERROR("GLAD not loaded properly in DLL.");
    //    return;
    //}

    //Serialize the game
    if(engine->input->keys[KEYS::F10]){
        serializeGame(gameState, "test");
    }
    if(engine->input->keys[KEYS::F11]){
        //ecsDestroy(gameState->ecs);
        gameStop(engine, gameState);
        gameState->ecs = initEcs();
        //gameState = gameStart(gameState, renderer);
        //TODO: clear the gameState before reserialize or leak memory
        deserializeGame(engine, gameState, "test");
        return;
    }

    //-------------------Physics----------------
    //systemPlayerEnemyCollision(gameState, gameState->ecs, dt);
    systemCollision(gameState, gameState->ecs, dt);
    //------------------------------------------

    inputSystem(gameState, gameState->ecs, engine->input, {ECS_SPRITE, ECS_VELOCITY, ECS_INPUT, ECS_DIRECTION}, dt);
    inputSystemWeapon(gameState, gameState->ecs, engine->input, {ECS_HITBOX, ECS_INPUT, ECS_ATTACHED_ENTITY}, dt);
    enemyFollowPlayerSystem(gameState->ecs, {ECS_TRANSFORM, ECS_DIRECTION, ECS_ENEMY_TAG}, dt);
    moveSystem(gameState->ecs, {ECS_TRANSFORM, ECS_VELOCITY}, dt);
    cameraFollowSystem(gameState->ecs, &gameState->camera, {ECS_PLAYER_TAG});
    animationSystem(gameState, gameState->ecs, {ECS_SPRITE, ECS_ANIMATION}, dt);
    systemUpdateAttachedEntity(gameState->ecs, {ECS_TRANSFORM, ECS_SPRITE, ECS_ATTACHED_ENTITY});
    PROFILER_END();
}


GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt){
    //if (!gladLoadGL()) {
    //    LOGERROR("GLAD not loaded properly in DLL.");
    //    return;
    //}
    PROFILER_START();
    renderTileMap(engine->renderer, gameState->bgMap, gameState->camera, 0.0f, false);
    systemRenderSprites(gameState, gameState->ecs, engine->renderer, {ECS_TRANSFORM, ECS_SPRITE}, dt);
    renderTileMap(engine->renderer, gameState->fgMap, gameState->camera, 2.0f, false);
    systemRenderColliders(gameState, gameState->ecs, engine->renderer, {ECS_2D_BOX_COLLIDER}, dt);
    systemRenderHitBox(gameState, gameState->ecs, engine->renderer, {ECS_HITBOX}, dt);
    systemRenderHurtBox(gameState, gameState->ecs, engine->renderer, {ECS_HURTBOX}, dt);
    //TODO: do attached component that store the id of the entity to which is attached and calculate position relative to it
    PROFILER_END();
}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    if(!gameState){
        return;
    }
    ecsDestroy(gameState->ecs);
    //destroyTextureManager(gameState->textureManager);
    //free(gameState);
    //delete gameState;
    PROFILER_CLEANUP();
}