#include "projectx.hpp"
#include "player.hpp"
#include "boss.hpp"
#include "projectile.hpp"
#include "spike.hpp"
#include "lifetime.hpp"

#define ACTIVE_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 255.0f / 255.0f, 255.0f  /255.0f)
#define DEACTIVE_COLLIDER_COLOR glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f)
#define HIT_COLLIDER_COLOR glm::vec4(0 , 255.0f / 255.0f, 0, 255.0f  /255.0f)
#define HURT_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 0, 255.0f / 255.0f)

void systemRenderColliders(GameState* gameState, Ecs* ecs, Renderer* renderer, float dt){
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, Box2DCollider);

    for(Entity entity : entities){
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider* box= getComponent(ecs, entity, Box2DCollider);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        Box2DCollider b = calculateWorldAABB(t, box);
        //if(box->active){
            renderDrawRect(b.offset, b.size, ACTIVE_COLLIDER_COLOR, 30);
        //}else{
        //    renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}
void systemRenderHitBox(GameState* gameState, Ecs* ecs, Renderer* renderer,float dt){
    std::vector<Entity> entities = view(ecs, HitBox, TransformComponent);

    for(Entity entity : entities){
        HitBox* hitBox= getComponent(ecs, entity, HitBox);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hit = calculateCollider(t, hitBox->offset, hitBox->size);
        //if(hitBox->area.active){
            renderDrawRect(hit.offset, hit.size, HIT_COLLIDER_COLOR, 30);
        //}else{
            //renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}

void systemRenderHurtBox(GameState* gameState, Ecs* ecs, Renderer* renderer, float dt){
    std::vector<Entity> entities = view(ecs, HurtBox, TransformComponent);

    for(Entity entity : entities){
        HurtBox* hurtBox= getComponent(ecs, entity, HurtBox);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hurt = calculateCollider(t, hurtBox->offset, hurtBox->size);
        //if(hurtBox->area.active){
            renderDrawRect(hurt.offset, hurt.size, HURT_COLLIDER_COLOR, 30);
        //}else{
            //renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}

void systemCollision(Ecs* ecs, float dt){

    systemCheckCollisions(ecs, dt);
    systemResolvePhysicsCollisions(ecs, dt);

    systemProjectileHit(ecs, dt);
    systemSpikeHit(ecs, dt);
    systemRespondBossHitStaticEntity(ecs, dt);
}


void systemRenderSprites(GameState* gameState, Ecs* ecs, Renderer* renderer, float dt){
    std::vector<Entity> entities = view(ecs, TransformComponent, SpriteComponent);

    for(Entity entity : entities){
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, TransformComponent);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, SpriteComponent);
        if(s->visible){
            renderDrawSprite(t->position, t->scale, t->rotation, s);
        }
    }
}

void moveSystem(Ecs* ecs, float dt){
    auto entities = view(ecs, TransformComponent, VelocityComponent, DirectionComponent);
    for(Entity e : entities){
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
        VelocityComponent* velocity  = (VelocityComponent*)  getComponent(ecs, e, VelocityComponent);
        DirectionComponent* direction  = (DirectionComponent*)  getComponent(ecs, e, DirectionComponent);
        transform->position += glm::vec3(direction->dir.x * velocity->vel.x * dt, direction->dir.y * velocity->vel.y * dt, transform->position.z);
    }
}

void deathSystem(Ecs* ecs){
    auto entities = view(ecs, HurtBox, BossTag);
    for(Entity e : entities){
        HurtBox* hurtbox = getComponent(ecs, e, HurtBox);
        if(hurtbox->health <= 0){
            removeEntity(ecs, e);
        }
    }
}

void gameOverSystem(Ecs* ecs, GameState* gameState){
    auto entities = view(ecs, HurtBox, PlayerTag);
    for(Entity e : entities){
        HurtBox* hurtbox = getComponent(ecs, e, HurtBox);
        if(hurtbox->health <= 0){
            gameState->gameLevels = GameLevels::GAME_OVER;
        }
    }
}

//NOTE: forward declaration
void loadLevel(GameState* gameState, EngineState* engine, GameLevels level);

void secondLevelSystem(Ecs* ecs, EngineState* engine, GameState* gameState){
    auto player = view(ecs, PlayerTag, Box2DCollider);
    auto portal = view(ecs, PortalTag, Box2DCollider);

    for(Entity entityA : player){
        Box2DCollider* boxAent = getComponent(ecs, entityA, Box2DCollider);
        for(Entity entityB : portal){
            Box2DCollider* boxBent = getComponent(ecs, entityB, Box2DCollider);
            if(beginCollision(entityA , entityB)){
                gameState->gameLevels = GameLevels::SECOND_LEVEL;
                loadLevel(gameState, engine, GameLevels::SECOND_LEVEL);
                break;
            }
        }
    }
}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera){
    std::vector<Entity> entities = view(ecs, PlayerTag);
    for(Entity entity : entities){
        TransformComponent* t = getComponent(ecs, entity, TransformComponent);
        if(!t){ break; }
        followTarget(camera, t->position);
    }
}

void animationSystem(Ecs* ecs, float dt){
    std::vector<Entity> entities = view(ecs, SpriteComponent, AnimationComponent);

    //NOTE: It should not be a system or it runs every frame and so even if the animation
    // is not showing it's been computed
    for(Entity entity : entities){
        SpriteComponent* s= getComponent(ecs, entity, SpriteComponent);
        AnimationComponent* animComp = getComponent(ecs, entity, AnimationComponent);
        Animation* anim = getAnimation(animComp->animationId);

        //if(component->id != component->previousId){ //NOTE: synchronize animation to frame 0 when it changes
        //    component->currentFrame = 0;
        //    component->previousId = component->id;
        //}

        if(anim->loop){
            if(anim->elapsedTime >= anim->frameDuration){
                anim->currentFrame = (anim->currentFrame + 1) % (anim->frames); // module to loop around
                anim->elapsedTime = 0;
                //component->frameCount = 0;
            }
        }else{
            if(anim->elapsedTime >= anim->frameDuration){
                anim->currentFrame = anim->currentFrame + 1; 
                anim->elapsedTime = 0;
                //component->frameCount = 0;
            }
        }
        anim->elapsedTime += dt;
        s->index = anim->indices[anim->currentFrame];
    }
}

struct WallTag{};
struct GamepadSpriteTag{};
struct PortalTag{};

void loadLevel(GameState* gameState, EngineState* engine, GameLevels level){
    PROFILER_START();
    clearEcs(engine->ecs);
    switch(level){
        case GameLevels::MAIN_MENU:{
            Entity gamepadSprite = createEntity(engine->ecs);
            SpriteComponent s = {
                .texture = getTexture("XOne"),
                .pivot = SpriteComponent::PIVOT_BOT_LEFT,
                .index = {0,0},
                .size = {320, 180},
                .visible = true
            };
            glm::vec3 gamepadPos = glm::vec3((gameState->camera.width / 2) - (320 / 2), (gameState->camera.height / 2) - (180 / 2), 0);
            TransformComponent t = {gamepadPos, glm::vec3(1,1,1), glm::vec3(0,0,0)};
            GamepadSpriteTag tag = {};
            pushComponent(engine->ecs, gamepadSprite, SpriteComponent, &s);
            pushComponent(engine->ecs, gamepadSprite, TransformComponent, &t);
            pushComponent(engine->ecs, gamepadSprite, GamepadSpriteTag, &tag);
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            gameState->bgMap = LoadTilesetFromTiled("test", engine->ecs);
            createPlayer(engine->ecs, engine, gameState->camera);
            //TODO: make default values for the components
            //directly in hpp file or just make utility functions to create the components???
            TransformComponent transform = {    
                .position = {50.0f, 50.0f, 0.0f},
                .scale = {1.0f, 1.0f, 0.0f},
                .rotation = {0.0f, 0.0f, 0.0f}
            };
            SpriteComponent sprite = {
                .texture = getTexture("dungeon"),
                .index = {15,8},
                .size = {32,32},
                .tileSize = {16, 16},
                .ySort = true,
                .layer = 1.0f
            };
            Box2DCollider coll = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {32, 32}, .isTrigger = true};
            PortalTag p = {};
            Entity portal = createEntity(engine->ecs);
            //transform.position = {0,0,0};
            //sprite.size = {10, gameState->camera.height};
            pushComponent(engine->ecs, portal, TransformComponent, &transform);
            pushComponent(engine->ecs, portal, Box2DCollider, &coll);
            pushComponent(engine->ecs, portal, SpriteComponent, &sprite);
            pushComponent(engine->ecs, portal, PortalTag, &p);
            break;
        }
        case GameLevels::SECOND_LEVEL:{
            gameState->camera.position = {0,0,0};
            gameState->camera.view = glm::mat4(1.0f);
            //Load Sound
            playAudio("sfx/celeste-test.ogg");

            //gameState->ecs = initEcs();

            createPlayer(engine->ecs, engine, gameState->camera);

            for(int i = 0; i < 1; i++){
                Entity boss = createBoss(engine->ecs, engine, gameState->camera, glm::vec3(i*50, i*30, 0));
            }

            //Walls
            {
                WallTag wallTag = {};
                //TODO: make default values for the components
                //directly in hpp file or just make utility functions to create the components???
                TransformComponent transform = {    
                    .position = {50.0f, 50.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                SpriteComponent sprite = {
                    .texture = getTexture("default"),
                    .index = {0,0},
                    .size = {16, 16},
                    .ySort = true,
                    .layer = 1.0f
                };
                Box2DCollider collider = {};
                Entity leftEdge = createEntity(engine->ecs);
                transform.position = {0,0,0};
                sprite.size = {10, gameState->camera.height};
                collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
                pushComponent(engine->ecs, leftEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, leftEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, leftEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, leftEdge, WallTag, &wallTag);
                Entity rightEdge = createEntity(engine->ecs);
                transform.position = {gameState->camera.width - 10,0,0};
                sprite.size = {10, gameState->camera.height};
                collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
                pushComponent(engine->ecs, rightEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, rightEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, rightEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, rightEdge, WallTag, &wallTag);
                Entity bottomEdge = createEntity(engine->ecs);
                transform.position = {0,0,0};
                sprite.size = {gameState->camera.width, 10};
                collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {gameState->camera.width, 10}};
                pushComponent(engine->ecs, bottomEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, bottomEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, bottomEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, bottomEdge, WallTag, &wallTag);
                Entity topEdge = createEntity(engine->ecs);
                transform.position = {0,gameState->camera.height - 10,0};
                sprite.size = {gameState->camera.width, 10};
                collider = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {gameState->camera.width, 10}};
                pushComponent(engine->ecs, topEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, topEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, topEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, topEdge, WallTag, &wallTag);
            }
            //UI
            //button("ciao Mondo", {10,10}, {200,200});
            break;
        }
        default:
            break;
    }
    PROFILER_END();
}

GAME_API void gameStart(EngineState* engine){
    //Always do that right now, i need to figure out how to remove this block of code
     if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    PROFILER_START();

    registerComponent(engine->ecs, TransformComponent);
    registerComponent(engine->ecs, SpriteComponent);
    registerComponent(engine->ecs, DirectionComponent);
    registerComponent(engine->ecs, VelocityComponent);
    registerComponent(engine->ecs, Box2DCollider);
    registerComponent(engine->ecs, PlayerTag);
    registerComponent(engine->ecs, ProjectileTag);
    registerComponent(engine->ecs, BossTag);
    registerComponent(engine->ecs, HitBox);
    registerComponent(engine->ecs, HurtBox);
    registerComponent(engine->ecs, SpikeTag);
    registerComponent(engine->ecs, LifeTime);
    registerComponent(engine->ecs, WallTag);
    registerComponent(engine->ecs, GamepadSpriteTag);
    registerComponent(engine->ecs, PortalTag);
    registerComponent(engine->ecs, AnimationComponent);

    GameState* gameState = new GameState();
    gameState->gameLevels = GameLevels::MAIN_MENU;
    engine->gameState = gameState;
    gameState->camera = createCamera({0,0,0}, 640, 320);
    loadAudio("sfx/celeste-test.ogg");
    loadFont(engine->fontManager, "Minecraft");
    loadTexture("Golem-hurt");
    loadTexture("idle-walk");
    loadTexture("XOne");
    loadTexture("tileset01");
    loadTexture("dungeon");
    loadTexture("idle-walk");

    //TileSet simple = createTileSet(getTexture(engine->textureManager, "tileset01"), 32, 32);
    //std::vector<int> tileBg = loadTilemapFromFile("assets/map/map-bg.csv", simple, 30);
    //std::vector<int> tileFg = loadTilemapFromFile("assets/map/map-fg.csv", simple, 30);


    //gameState->bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    //gameState->fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    //-----------------------------------------------------------------------------------
    loadLevel(gameState, engine, GameLevels::MAIN_MENU);
    PROFILER_END();
}

GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt){
    PROFILER_START();
    static float updateText = 0.2;
    static float timer = 0;
    static float ffps = 0;
    beginScene(&gameState->camera);
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    animationSystem(engine->ecs, dt);
    systemRenderSprites(gameState, engine->ecs, engine->renderer, dt);

    if(gameState->debugMode){
        systemRenderColliders(gameState, engine->ecs, engine->renderer, dt);
        systemRenderHitBox(gameState, engine->ecs, engine->renderer, dt);
        systemRenderHurtBox(gameState, engine->ecs, engine->renderer, dt);
    }
    switch (gameState->gameLevels)
    {
        case GameLevels::MAIN_MENU:{
            auto startMenuSprites = view(engine->ecs, GamepadSpriteTag, SpriteComponent, TransformComponent);
            glm::vec3 gamepadPos = getComponent(engine->ecs, startMenuSprites[0], TransformComponent)->position;
            //TODO: make text as entities in order to remove this switch case and use the switch only in levelManager(sceneManager)
            renderDrawText(getFont(engine->fontManager, "Minecraft"),
                        gameState->camera, "Press Start to play the Game!!!",
                        gamepadPos.x ,
                        gamepadPos.y - 6,
                        0.5);
            break;
        }
        case GameLevels::FIRST_LEVEL:
            PROFILER_SCOPE_START("RenderTilemap");
            //renderDrawQuad({10,10,10},{1,1,1},{0,0,0}, getTexture("XOne"), {0,0}, {200,200}, false);
            renderTileMap(&gameState->bgMap);
            PROFILER_SCOPE_END();
            //renderTileSet(engine->renderer, gameState->bgMap.tileset, gameState->camera);
            //renderTileMap(engine->renderer, gameState->fgMap, gameState->camera, 1.0f, true);
            break;
        case GameLevels::SECOND_LEVEL:
            break;

        case GameLevels::GAME_OVER:
            clearColor(1.0f, 0.3f, 0.3f, 1.0f);
            renderDrawText(getFont(engine->fontManager, "Minecraft"),
                        gameState->camera, "GAME OVER!",
                        (gameState->camera.width  / 2) - 120,
                        (gameState->camera.height / 2) - 24,
                        1.0);
            break;
    }
    endScene();

    //renderDrawFilledRect(engine->renderer, gameState->camera, {50,50,0}, {100,100}, {0,0,0}, {1,0,0,0.25});

    timer += dt;
    if(timer >= updateText){
        ffps = engine->fps;
        timer = 0;
    }
    renderDrawText(getFont(engine->fontManager, "ProggyClean"),
                gameState->camera, std::to_string(ffps).c_str(),
                gameState->camera.width -500 ,
                gameState->camera.height - 40,
                1.0);
    //UI
    //renderUIElements();
    PROFILER_END();
}

GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt){
    PROFILER_START();
    if(isJustPressed(engine->input, KEYS::F5)){
        gameState->debugMode = !gameState->debugMode;
    }
    switch (gameState->gameLevels)
    {
        case GameLevels::MAIN_MENU:
            if(isJustPressedGamepad(&engine->input->gamepad, GAMEPAD_BUTTON_START)){
                gameState->gameLevels = GameLevels::FIRST_LEVEL;
                loadLevel(gameState, engine, GameLevels::FIRST_LEVEL);
            }
            break;
        case GameLevels::FIRST_LEVEL:
            animateTiles(&gameState->bgMap, dt);
            systemCollision(engine->ecs, dt);
            deathSystem(engine->ecs);
            inputPlayerSystem(engine->ecs, engine, engine->input);
            moveSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &gameState->camera);
            secondLevelSystem(engine->ecs, engine, gameState);
            break;
        case GameLevels::SECOND_LEVEL:
            systemCollision(engine->ecs, dt);
            gameOverSystem(engine->ecs, gameState);
            systemCheckRange(engine->ecs, dt);
            bossAiSystem(engine->ecs, engine, gameState->camera, dt);
            deathSystem(engine->ecs);
            moveSystem(engine->ecs, dt);
            inputPlayerSystem(engine->ecs, engine, engine->input);
            lifeTimeSystem(engine->ecs, dt);
            changeBossTextureSystem(engine->ecs);
            break;
        case GameLevels::GAME_OVER:
            break;
    }
    PROFILER_END();
}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    PROFILER_CLEANUP();
    clearEcs(engine->ecs);
    delete gameState;
}