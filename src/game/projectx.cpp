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
    PROFILER_START();
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, Box2DCollider);
    //Box2DCollider* box= getComponentVector(ecs, Box2DCollider);
    //TransformComponent* t= getComponentVector(ecs, TransformComponent);

    for(Entity entity : entities){
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider* box= getComponent(ecs, entity, Box2DCollider);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        Box2DCollider b = calculateWorldAABB(t, box);
        //if(box->active){
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, ACTIVE_COLLIDER_COLOR, 30);
        //}else{
        //    renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
    PROFILER_END();
}
void systemRenderHitBox(GameState* gameState, Ecs* ecs, Renderer* renderer,float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, HitBox, TransformComponent);

    for(Entity entity : entities){
        HitBox* hitBox= getComponent(ecs, entity, HitBox);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hit = calculateCollider(t, hitBox->offset, hitBox->size);
        //if(hitBox->area.active){
            renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, HIT_COLLIDER_COLOR, 30);
        //}else{
            //renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
    PROFILER_END();
}

void systemRenderHurtBox(GameState* gameState, Ecs* ecs, Renderer* renderer, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, HurtBox, TransformComponent);

    for(Entity entity : entities){
        HurtBox* hurtBox= getComponent(ecs, entity, HurtBox);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hurt = calculateCollider(t, hurtBox->offset, hurtBox->size);
        //if(hurtBox->area.active){
            renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, HURT_COLLIDER_COLOR, 30);
        //}else{
            //renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
    PROFILER_END();
}

//void systemCheckHitBox(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
//    for(Entity entityA : entitiesA){
//        HitBox* boxAent= getComponent(ecs, entityA, HitBox);
//        TransformComponent* tA= getComponent(ecs, entityA, TransformComponent);
//        for(Entity entityB : entitiesB){
//            if(entityA == entityB) continue; //skip self collision
//
//            HurtBox* boxBent = getComponent(ecs, entityB, HurtBox);
//            TransformComponent* tB = getComponent(ecs, entityB, TransformComponent);
//            //I need the position of the box which is dictated by the entity position + the box offset
//            Box2DCollider boxA = calculateCollider(tA, boxAent->offset, boxAent->size); 
//            Box2DCollider boxB = calculateCollider(tB, boxBent->offset, boxBent->size); 
//
//            //if(boxAent->area.active && boxBent->area.active && isColliding(&boxA, &boxB)){
//            if(isColliding(&boxA, &boxB)){
//                //if(!boxAent->alreadyHitted){
//                //    boxBent->health -= boxAent->dmg;
//                //    boxAent->alreadyHitted = true;
//                //    boxBent->hitted = true;
//                //}
//                //boxAent->discover = true;
//            }
//        }
//    }
//}
//


void systemCollision(Ecs* ecs, float dt){
    PROFILER_START();

    systemCheckCollisions(ecs, dt);
    systemResolvePhysicsCollisions(ecs, dt);

    systemProjectileHit(ecs, dt);
    systemSpikeHit(ecs, dt);
    systemRespondBossHitStaticEntity(ecs, dt);
    PROFILER_END();
}


void systemRenderSprites(GameState* gameState, Ecs* ecs, Renderer* renderer, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, TransformComponent, SpriteComponent);

    for(Entity entity : entities){
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, TransformComponent);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, SpriteComponent);
        if(s->visible){
            renderDrawSprite(renderer, gameState->camera, t->position, t->scale, t->rotation, s);
        }
    }

    //SpriteComponent* s = getComponentVector(ecs, SpriteComponent);
    //TransformComponent* t= getComponentVector(ecs, TransformComponent);
    //for(Entity e : entities){
    //    if(s[e].visible){
    //        renderDrawSprite(renderer, gameState->camera, t[e].position, t[e].scale, t[e].rotation, &s[e]);
    //    }
    //}
    PROFILER_END();
}

void moveSystem(Ecs* ecs, float dt){
    auto entities = view(ecs, TransformComponent, VelocityComponent, DirectionComponent);
    //TransformComponent* transform = getComponentVector(ecs, TransformComponent);
    //VelocityComponent* velocity  = getComponentVector(ecs, VelocityComponent);
    //DirectionComponent* direction  = getComponentVector(ecs, DirectionComponent);
    //for(Entity e : entities){
    //    transform[e].position += glm::vec3(direction[e].dir.x * velocity[e].vel.x * dt, direction[e].dir.y * velocity[e].vel.y * dt, transform[e].position.z);
    //}
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
            gameState->gameOver = true;
        }
    }

}

struct WallTag{};

GAME_API void gameStart(EngineState* engine){
    //Always do that right now, i need to figure out how to remove this block of code
     if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    GameState* gameState = new GameState();
    engine->gameState = gameState;
    //-----------------------------------------------------------------------------------

    gameState->ecs = initEcs();
    gameState->camera = createCamera({0,0,0}, 640, 320);
    loadFont(engine->fontManager, "Minecraft");

    loadTexture(engine->textureManager, "Golem-hurt");

    registerComponent(gameState->ecs, TransformComponent);
    registerComponent(gameState->ecs, SpriteComponent);
    registerComponent(gameState->ecs, DirectionComponent);
    registerComponent(gameState->ecs, VelocityComponent);
    registerComponent(gameState->ecs, Box2DCollider);
    registerComponent(gameState->ecs, PlayerTag);
    registerComponent(gameState->ecs, ProjectileTag);
    registerComponent(gameState->ecs, BossTag);
    registerComponent(gameState->ecs, HitBox);
    registerComponent(gameState->ecs, HurtBox);
    registerComponent(gameState->ecs, SpikeTag);
    registerComponent(gameState->ecs, LifeTime);

    createPlayer(gameState->ecs, engine, gameState->camera);

    for(int i = 0; i < 1; i++){
        Entity boss = createBoss(gameState->ecs, engine, gameState->camera, glm::vec3(i*50, i*30, 0));
    }

    //Walls
    {
        registerComponent(gameState->ecs, WallTag);
        WallTag wallTag = {};
        //TODO: make default values for the components
        //directly in hpp file or just make utility functions to create the components???
        TransformComponent transform = {    
            .position = {50.0f, 50.0f, 0.0f},
            .scale = {1.0f, 1.0f, 0.0f},
            .rotation = {0.0f, 0.0f, 0.0f}
        };
        SpriteComponent sprite = {
            .texture = getTexture(engine->textureManager, "default"),
            .index = {0,0},
            .size = {16, 16},
            .ySort = true,
            .layer = 1.0f
        };
        Box2DCollider collider = {};
        sprite.color = {0,0,1,1};
        Entity leftEdge = createEntity(gameState->ecs);
        transform.position = {0,0,0};
        sprite.size = {10, gameState->camera.height};
        collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
        pushComponent(gameState->ecs, leftEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, leftEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, leftEdge, SpriteComponent, &sprite);
        pushComponent(gameState->ecs, leftEdge, WallTag, &wallTag);
        Entity rightEdge = createEntity(gameState->ecs);
        transform.position = {gameState->camera.width - 10,0,0};
        sprite.size = {10, gameState->camera.height};
        collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
        pushComponent(gameState->ecs, rightEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, rightEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, rightEdge, SpriteComponent, &sprite);
        pushComponent(gameState->ecs, rightEdge, WallTag, &wallTag);
        Entity bottomEdge = createEntity(gameState->ecs);
        transform.position = {0,0,0};
        sprite.size = {gameState->camera.width, 10};
        collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {gameState->camera.width, 10}};
        pushComponent(gameState->ecs, bottomEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, bottomEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, bottomEdge, SpriteComponent, &sprite);
        pushComponent(gameState->ecs, bottomEdge, WallTag, &wallTag);
        Entity topEdge = createEntity(gameState->ecs);
        transform.position = {0,gameState->camera.height - 10,0};
        sprite.size = {gameState->camera.width, 10};
        collider = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {gameState->camera.width, 10}};
        pushComponent(gameState->ecs, topEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, topEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, topEdge, SpriteComponent, &sprite);
        pushComponent(gameState->ecs, topEdge, WallTag, &wallTag);
    }

    //UI
    //button("ciao Mondo", {10,10}, {200,200});
}

GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt){
    static float updateText = 0.2;
    static float timer = 0;
    static float ffps = 0;

    if(gameState->gameOver){
        clearColor(1.0f, 0.3f, 0.3f, 1.0f);
        renderDrawText(engine->renderer, getFont(engine->fontManager, "Minecraft"),
                    gameState->camera, "GAME OVER!",
                    (gameState->camera.width  / 2) - 120,
                    (gameState->camera.height / 2) - 24,
                    1.0);
        return;
    }

    systemRenderSprites(gameState, gameState->ecs, engine->renderer, dt);

    if(gameState->debugMode){
        systemRenderColliders(gameState, gameState->ecs, engine->renderer, dt);
        systemRenderHitBox(gameState, gameState->ecs, engine->renderer, dt);
        systemRenderHurtBox(gameState, gameState->ecs, engine->renderer, dt);
    }
    //renderDrawFilledRect(engine->renderer, gameState->camera, {50,50,0}, {100,100}, {0,0,0}, {1,0,0,0.25});

    timer += dt;
    if(timer >= updateText){
        ffps = engine->fps;
        timer = 0;
    }
    renderDrawText(engine->renderer, getFont(engine->fontManager, "ProggyClean"),
                gameState->camera, std::to_string(ffps).c_str(),
                gameState->camera.width -500 ,
                gameState->camera.height - 40,
                1.0);
    //UI
    //renderUIElements();
}

GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt){
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gameOverSystem(gameState->ecs, gameState);
    if(gameState->gameOver){ return; }
    systemCheckRange(gameState->ecs, dt);
    bossAiSystem(gameState->ecs, engine, gameState->camera, dt);
    deathSystem(gameState->ecs);
    moveSystem(gameState->ecs, dt);
    inputPlayerSystem(gameState->ecs, engine, engine->input);
    lifeTimeSystem(gameState->ecs, dt);
    changeBossTextureSystem(gameState->ecs);

    systemCollision(gameState->ecs, dt);

    if(isJustPressed(engine->input, KEYS::F5)){
        gameState->debugMode = !gameState->debugMode;
    }
}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    delete gameState;
}