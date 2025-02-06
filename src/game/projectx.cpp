#include "projectx.hpp"
#include "player.hpp"
#include "projectile.hpp"

#define ACTIVE_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 255.0f / 255.0f, 255.0f  /255.0f)
#define DEACTIVE_COLLIDER_COLOR glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f)

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
        if(box->active){
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, ACTIVE_COLLIDER_COLOR, 30);
        }else{
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        }
    }
    PROFILER_END();
}

void systemCollision(Ecs* ecs, float dt){
    PROFILER_START();
    std::vector<Entity> dynamicEntities;
    std::vector<Entity> staticEntities;
    std::vector<Entity> colliderEntities = view(ecs, Box2DCollider);
    Box2DCollider* collider = getComponentVector(ecs, Box2DCollider);
    for(Entity e : colliderEntities){
        if(collider[e].type == Box2DCollider::STATIC && collider[e].active){
            staticEntities.push_back(e);
        }else if(collider[e].type == Box2DCollider::DYNAMIC && collider[e].active){
            dynamicEntities.push_back(e);
        }
    }
    //systemCheckCollisionDynamicDynamic(ecs, dynamicEntities, dynamicEntities, dt);
    systemCheckCollisionDynamicStatic(ecs, dynamicEntities, staticEntities, dt);

    //std::vector<Entity> weaponHitBoxes = view(ecs, {ECS_HITBOX, ECS_WEAPON});
    //std::vector<Entity> enemyHitBoxes = view(ecs, {ECS_HITBOX, ECS_ENEMY_TAG});
    //std::vector<Entity> playerHurtBoxes = view(ecs, {ECS_HURTBOX, ECS_PLAYER_TAG});
    //std::vector<Entity> monsterHurtBoxes = view(ecs, {ECS_HURTBOX, ECS_ENEMY_TAG});
    //systemCheckHitBox(ecs, weaponHitBoxes, monsterHurtBoxes, dt);
    //systemCheckHitBox(ecs, enemyHitBoxes, playerHurtBoxes, dt);
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
    gameState->camera = createCamera({0,0,0}, 640, 360);
    loadFont(engine->fontManager, "Minecraft");
    
    //Components c = initComponents(sizeof(VelocityComponent));
    //for(int i = 0 ; i < 100; i++){
    //    VelocityComponent v = {.vel={i,i}};
    //    push_back(&c, & v);
    //}

    //VelocityComponent* v2 = (VelocityComponent*)c.elements;
    //for(int i = 0; i < c.count; i++){
    //    //VelocityComponent* v1 = (VelocityComponent*)get(&c, i);
    //    //LOGINFO("%f/%f", v1->vel.x, v1->vel.y);
    //    LOGINFO("%f/%f", v2[i].vel.x, v2[i].vel.y);
    //}

    //registerComponent(gameState->ecs, ECS_TRANSFORM, sizeof(TransformComponent));
    //registerComponent(gameState->ecs, ECS_SPRITE, sizeof(SpriteComponent));
    //registerComponent(gameState->ecs, ECS_DIRECTION, sizeof(DirectionComponent));
    //registerComponent(gameState->ecs, ECS_VELOCITY, sizeof(VelocityComponent));
    //registerComponent(gameState->ecs, ECS_2D_BOX_COLLIDER, sizeof(Box2DCollider));

    registerComponent(gameState->ecs, TransformComponent);
    registerComponent(gameState->ecs, SpriteComponent);
    registerComponent(gameState->ecs, DirectionComponent);
    registerComponent(gameState->ecs, VelocityComponent);
    registerComponent(gameState->ecs, Box2DCollider);
    registerComponent(gameState->ecs, PlayerTag);
    registerComponent(gameState->ecs, ProjectileTag);


    createPlayer(gameState->ecs, engine);

    //Walls
    {
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
        Entity rightEdge = createEntity(gameState->ecs);
        transform.position = {gameState->camera.width - 10,0,0};
        sprite.size = {10, gameState->camera.height};
        collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
        pushComponent(gameState->ecs, rightEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, rightEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, rightEdge, SpriteComponent, &sprite);
        Entity bottomEdge = createEntity(gameState->ecs);
        transform.position = {0,0,0};
        sprite.size = {gameState->camera.width, 10};
        collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {gameState->camera.width, 10}};
        pushComponent(gameState->ecs, bottomEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, bottomEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, bottomEdge, SpriteComponent, &sprite);
        Entity topEdge = createEntity(gameState->ecs);
        transform.position = {0,gameState->camera.height - 10,0};
        sprite.size = {gameState->camera.width, 10};
        collider = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {gameState->camera.width, 10}};
        pushComponent(gameState->ecs, topEdge, TransformComponent, &transform);
        pushComponent(gameState->ecs, topEdge, Box2DCollider, &collider);
        pushComponent(gameState->ecs, topEdge, SpriteComponent, &sprite);


        Entity test = createEntity(gameState->ecs);
        transform.position = {200,200,0};
        sprite.size = {300, 100};
        collider = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {300, 100}};
        pushComponent(gameState->ecs, test, TransformComponent, &transform);
        pushComponent(gameState->ecs, test, Box2DCollider, &collider);
        pushComponent(gameState->ecs, test, SpriteComponent, &sprite);
    }

    //UI
    //button("ciao Mondo", {10,10}, {200,200});
}

GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt){
    static float updateText = 0.2;
    static float timer = 0;
    static float ffps = 0;

    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //clearColor(1.0f, 0.3f, 0.3f, 1.0f);
    systemRenderSprites(gameState, gameState->ecs, engine->renderer, dt);
    systemRenderColliders(gameState, gameState->ecs, engine->renderer, dt);

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

    systemCollision(gameState->ecs, dt);
    moveSystem(gameState->ecs, dt);
    inputPlayerSystem(gameState->ecs, engine, engine->input);

}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    delete gameState;
}