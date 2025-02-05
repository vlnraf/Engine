#include "projectx.hpp"

#include "player.hpp"

void systemRenderSprites(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
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

void moveSystem(Ecs* ecs, float dt){
    auto entities = view(ecs, {ECS_TRANSFORM, ECS_VELOCITY, ECS_DIRECTION});
    //TransformComponent* transform = (TransformComponent*) getComponentVector(ecs, ECS_TRANSFORM);
    //VelocityComponent* velocity  = (VelocityComponent*)  getComponentVector(ecs, ECS_VELOCITY);
    //DirectionComponent* direction  = (DirectionComponent*)  getComponentVector(ecs, ECS_DIRECTION);
    //for(Entity e : entities){
    //    transform[e].position += glm::vec3(direction[e].dir.x * velocity[e].vel.x * dt, direction[e].dir.y * velocity[e].vel.y * dt, transform[e].position.z);
    //}
    for(Entity e : entities){
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, ECS_TRANSFORM);
        VelocityComponent* velocity  = (VelocityComponent*)  getComponent(ecs, e, ECS_VELOCITY);
        DirectionComponent* direction  = (DirectionComponent*)  getComponent(ecs, e, ECS_DIRECTION);
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

    //registerComponent(gameState->ecs, ECS_TRANSFORM, sizeof(TransformComponent));
    //registerComponent(gameState->ecs, ECS_SPRITE, sizeof(SpriteComponent));
    //registerComponent(gameState->ecs, ECS_DIRECTION, sizeof(DirectionComponent));
    //registerComponent(gameState->ecs, ECS_VELOCITY, sizeof(VelocityComponent));

    for(int i = 0; i < 1; i++){
        createPlayer(gameState->ecs, engine);
    }

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
        //collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.width}};
        pushComponent(gameState->ecs, leftEdge, ECS_TRANSFORM, &transform, sizeof(TransformComponent));
        pushComponent(gameState->ecs, leftEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
        pushComponent(gameState->ecs, leftEdge, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
        Entity rightEdge = createEntity(gameState->ecs);
        transform.position = {gameState->camera.width - 10,0,0};
        sprite.size = {10, gameState->camera.height};
        //collider = {.type = Box2DCollider::STATIC, .offset = {gameState->fgMap.width * gameState->fgMap.tileSize - 10, 0}, .size = {gameState->fgMap.width, gameState->fgMap.height * gameState->fgMap.tileSize}};
        pushComponent(gameState->ecs, rightEdge, ECS_TRANSFORM, &transform, sizeof(TransformComponent));
        pushComponent(gameState->ecs, rightEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
        pushComponent(gameState->ecs, rightEdge, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
        Entity bottomEdge = createEntity(gameState->ecs);
        transform.position = {0,0,0};
        sprite.size = {gameState->camera.width, 10};
        //collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {gameState->fgMap.width * gameState->fgMap.tileSize, gameState->fgMap.tileSize}};
        pushComponent(gameState->ecs, bottomEdge, ECS_TRANSFORM, &transform, sizeof(TransformComponent));
        pushComponent(gameState->ecs, bottomEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
        pushComponent(gameState->ecs, bottomEdge, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
        Entity topEdge = createEntity(gameState->ecs);
        transform.position = {0,gameState->camera.height - 10,0};
        sprite.size = {gameState->camera.width, 10};
        //collider = {.type = Box2DCollider::STATIC, .offset = {0, gameState->fgMap.height * gameState->fgMap.tileSize - 32}, .size = {gameState->fgMap.width * gameState->fgMap.tileSize, gameState->fgMap.tileSize}};
        pushComponent(gameState->ecs, topEdge, ECS_TRANSFORM, &transform, sizeof(TransformComponent));
        pushComponent(gameState->ecs, topEdge, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
        pushComponent(gameState->ecs, topEdge, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    }

    //UI
    //button("ciao Mondo", {10,10}, {200,200});
}

GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt){

    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //clearColor(1.0f, 0.3f, 0.3f, 1.0f);
    systemRenderSprites(gameState, gameState->ecs, engine->renderer, {ECS_TRANSFORM, ECS_SPRITE}, dt);

    renderDrawText(engine->renderer, getFont(engine->fontManager, "ProggyClean"),
                   gameState->camera, std::to_string(engine->fps).c_str(),
                   gameState->camera.width -500 ,
                   gameState->camera.height - 40,
                   1.0);

    //UI
    //renderUIElements();
}

GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt){
    inputPlayerSystem(gameState->ecs, engine->input);
    moveSystem(gameState->ecs, dt);
}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    delete gameState;
}