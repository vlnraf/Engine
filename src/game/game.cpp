#include <malloc.h>

#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


#define COLLIDER_COLOR glm::vec4(1, 0, 1, 1)

MyProfiler prof;

void systemRenderSprites(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        if(s->texture){
            renderDrawQuad(renderer, gameState->camera, t->position, t->scale, t->rotation, s->texture, s->index, s->size, s->layer);
        }
    }
    PROFILER_END();
}

void systemRenderColliders(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        Box2DCollider* box= (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        renderDrawRect(renderer, gameState->camera, box->offset, box->size, COLLIDER_COLOR, 0);
    }
    PROFILER_END();
}

void systemUpdateColliders(GameState* gameState, Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        Box2DCollider* box= (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        box->offset = t->position;
    }
    PROFILER_END();
}

void animationSystem(GameState* gameState, Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        AnimationComponent* component = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        Animation* animation = getAnimation(&gameState->animationManager, component->id.c_str());
        component->frameCount += dt;

        if(component->frameCount >= animation->frameDuration){
            component->currentFrame = (component->currentFrame + 1) % (animation->frames); // module to loop around
            component->frameCount = 0;
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

void inputSystem(GameState* gameState, Ecs* ecs, Input* input, std::vector<ComponentType> types){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        VelocityComponent* velocity = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
        //SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        AnimationComponent* data = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        direction->dir = {0,0};
        {   //GamePad
            //Animation* anim = getAnimation(&gameState->animationManager, data->id.c_str());//->animations.at("idleRight");
            //data->frameDuration = 1.0f / anim->frames;
            data->id = "idleLeft";
            if(input->gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER]){LOGINFO("Trigger Sinistro");}
            if(input->gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER]){LOGINFO("Trigger Destro");}
            if(abs(input->gamepad.leftX) > 0.1 || abs(input->gamepad.leftY) > 0.1){ //threshold because it's never 0.0
                //NOTE: Input entity can have no animation component, error prone
                //TODO: just implement the change of animation logic elsewhere
                //I think the input system is not the system where do this logic
                //And implement direction logic instead of threshold
                if(input->gamepad.leftX < -0.3){
                    direction->dir = {-1, 0};
                    data->id = "walkLeft";
                }else if(input->gamepad.leftX > 0.3){
                    direction->dir = {1, 0};
                    data->id = "walkRight";
                }else if(input->gamepad.leftY > 0.3){
                    direction->dir = {0, 1};
                    data->id = "walkTop";
                }else if(input->gamepad.leftY < -0.3){
                    direction->dir = {0, -1};
                    data->id = "walkBottom";
                }
            }
            setComponent(ecs, entity, data, ECS_ANIMATION);
        }
        if(input->keys[KEYS::W]){ 
            direction->dir.y = 1;
            velocity->vel.y = 100;
            data->id = "walkTop";
        }
        if(input->keys[KEYS::S]){
            direction->dir.y = -1;
            velocity->vel.y = 100;
            data->id = "walkBottom";
        }
        if(input->keys[KEYS::A]){ 
            direction->dir.x = -1;
            velocity->vel.x = 100;
            data->id = "walkLeft";
        }
        if(input->keys[KEYS::D]){ 
            direction->dir.x = 1;
            velocity->vel.x = 100;
            data->id = "walkRight";
        }
        //NOTE: should i normalize the direction???
        //direction->dir = glm::normalize(direction->dir);
    }
    PROFILER_END();
}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera, Entity id){
    PROFILER_START();
    TransformComponent* t = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);

    followTarget(camera, t->position);
    PROFILER_END();
}

void enemyFollowPlayerSystem(Ecs* ecs, Entity player, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    TransformComponent* playerT = (TransformComponent*) getComponent(ecs, player, ECS_TRANSFORM);
    glm::vec3 followPlayer = playerT->position;

    //check for the center bottom instead of left bottom point
    followPlayer.x = playerT->position.x;// + (0.5 * playerT->scale.x);
    //float dirX, dirY;
    for(Entity entity : entities){
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
        TransformComponent* t = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        DirectionComponent* dir = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);

        //dirX = followPlayer.x - t->position.x;// (t->position.x + (0.5 * t->scale.x));
        //dirY = followPlayer.y - t->position.y;
        dir->dir.x = followPlayer.x - t->position.x;
        dir->dir.y = followPlayer.y - t->position.y;
        dir->dir = glm::normalize(dir->dir);

        t->position.x += vel->vel.x * dir->dir.x * dt;
        t->position.y += vel->vel.y * dir->dir.y * dt;
        //t->position += glm::vec3(vel->vel.x, vel->vel.y, 0.0f);
    }
    PROFILER_END();

}


GAME_API GameState* gameStart(Renderer* renderer){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    PROFILER_SAVE("profiler.json");

    GameState* gameState = new GameState();
    PROFILER_START();
    gameState->ecs = initEcs();
    //TODO: make a resource manager
    //I think this also slow down the boot-up, so we can load textures with another thread
    Texture* wall = loadTexture("assets/sprites/wall.jpg");
    Texture* awesome = loadTexture("assets/sprites/awesomeface.png");
    Texture* white = getWhiteTexture();
    Texture* tileSet = loadTexture("assets/sprites/tileset01.png");
    Texture* hero = loadTexture("assets/sprites/hero.png");
    //Texture* idle = loadTexture("assets/idle.png");
    //Texture* walk = loadTexture("assets/walk.png");
    Texture* idleWalk = loadTexture("assets/idle-walk.png");
    Texture* treeSprite = loadTexture("assets/sprites/tree.png");

    TileSet simple = createTileSet(tileSet, 32);

    //std::vector<int> tileBg = {
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    //    };

    //std::vector<int> tileFg = {
    //    349, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 351,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    369, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 371,
    //    389, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 391,
    //    };

    std::vector<int> tileBg = loadTilemapFromFile("assets/map/map-bg.csv", simple, 30);
    std::vector<int> tileFg = loadTilemapFromFile("assets/map/map-fg.csv", simple, 30);

    gameState->bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    gameState->fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    TransformComponent transform = {};
    transform.position = glm ::vec3(10.0f, 10.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);

    SpriteComponent sprite = {};
    sprite.texture = white;

    InputComponent inputC = {};
    inputC.x = 0.0f;
    inputC.y = 0.0f;

    VelocityComponent velocity = {.vel = {0, 0}};

    DirectionComponent direction = {.dir = {1,0}};


    gameState->camera = createCamera(glm::vec3(0.0f, 0.0f, 0.0f), 640, 320);

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    //transform.scale = glm ::vec3(25.0f, 25.0f , 0.0f);
    transform.scale = glm ::vec3(2.0f, 2.0f, 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    uint32_t player = createEntity(gameState->ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    //sprite.id = awesome->id;
    sprite.texture = idleWalk;
    sprite.index = {0,0};
    sprite.size = {16, 16};
    sprite.layer = 2.0f;
    //Box2DCollider collider = {.offset = {transform.position.x, transform.position.y}, .size = {sprite.size.x, sprite.size.y}};
    Box2DCollider collider = {.offset = {transform.position.x, transform.position.y}, .size = {sprite.size.x * transform.scale.x, sprite.size.y * transform.scale.y}};

    //AnimationManager* animationManager = &gameState->animationManager;
    gameState->animationManager = initAnimationManager();

    {   //Animatioin registry
        registryAnimation(&gameState->animationManager, "idleRight", 4, (uint16_t[]){0,1,2,3}, 0);
        registryAnimation(&gameState->animationManager, "idleLeft", 4, (uint16_t[]){0,1,2,3}, 0);
        registryAnimation(&gameState->animationManager, "idleBottom", 4, (uint16_t[]){0,1,2,3}, 1);
        registryAnimation(&gameState->animationManager, "idleTop", 4, (uint16_t[]){0,1,2,3}, 2);
        registryAnimation(&gameState->animationManager, "walkRight", 8, (uint16_t[]){0,1,2,3}, 3);
        registryAnimation(&gameState->animationManager, "walkLeft", 8, (uint16_t[]){0,1,2,3}, 3);
        registryAnimation(&gameState->animationManager, "walkBottom", 8, (uint16_t[]){0,1,2,3}, 4);
        registryAnimation(&gameState->animationManager, "walkTop", 8, (uint16_t[]){0,1,2,3}, 5);
    }

    AnimationComponent anim = {};
    anim.id = "idleRight";

    pushComponent(gameState->ecs, player, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
    pushComponent(gameState->ecs, player, ECS_DIRECTION, (void*)&direction, sizeof(DirectionComponent));
    pushComponent(gameState->ecs, player, ECS_2D_BOX_COLLIDER, (void*)&collider, sizeof(Box2DCollider));
    pushComponent(gameState->ecs, player, ECS_INPUT, (void*)&inputC, sizeof(InputComponent));
    pushComponent(gameState->ecs, player, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
    pushComponent(gameState->ecs, player, ECS_ANIMATION, (void*)&anim, sizeof(AnimationComponent));
    gameState->player = player;

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    uint32_t tree = createEntity(gameState->ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    sprite.texture = treeSprite;
    sprite.index = {0,0};
    sprite.size = {treeSprite->width, treeSprite->height};
    pushComponent(gameState->ecs, tree, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));

    srand(time(NULL));

    for(int i = 0; i < 30; i++){
        transform.position = glm::vec3(rand() % 1200 + 32, rand() % 900 + 32, 0.0f);
        transform.scale = glm ::vec3(0.02f, 0.02f , 0.0f);
        transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
        uint32_t enemy = createEntity(gameState->ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
        sprite.texture = awesome;
        sprite.index = {0,0};
        sprite.size = {512, 512};
        Box2DCollider collider = {.offset = {transform.position.x, transform.position.y}, .size = {sprite.size.x * transform.scale.x, sprite.size.y * transform.scale.y}};
        velocity.vel = {20.0f, 20.0f};
        DirectionComponent direction = {.dir = {0,0}};
        EnemyComponent enemyComp = {};
        pushComponent(gameState->ecs, enemy, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
        pushComponent(gameState->ecs, enemy, ECS_DIRECTION, (void*)&direction, sizeof(DirectionComponent));
        pushComponent(gameState->ecs, enemy, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
        pushComponent(gameState->ecs, enemy, ECS_ENEMY, (void*)&enemyComp, sizeof(EnemyComponent));
        pushComponent(gameState->ecs, enemy, ECS_2D_BOX_COLLIDER, (void*)&collider, sizeof(Box2DCollider));
    }
    //removeEntity(gameState.ecs, player);
    PROFILER_END();
    return gameState;
}

GAME_API void gameUpdate(GameState* gameState, Input* input, float dt){
    PROFILER_START();
    inputSystem(gameState, gameState->ecs, input, {ECS_TRANSFORM, ECS_VELOCITY, ECS_INPUT, ECS_DIRECTION});
    moveSystem(gameState->ecs, {ECS_TRANSFORM, ECS_VELOCITY}, dt);
    cameraFollowSystem(gameState->ecs, &gameState->camera, gameState->player);
    enemyFollowPlayerSystem(gameState->ecs, gameState->player, {ECS_VELOCITY, ECS_TRANSFORM, ECS_DIRECTION, ECS_ENEMY}, dt);
    animationSystem(gameState, gameState->ecs, {ECS_SPRITE, ECS_ANIMATION}, dt);
    systemUpdateColliders(gameState, gameState->ecs, {ECS_TRANSFORM, ECS_2D_BOX_COLLIDER}, dt);
    PROFILER_END();
}

GAME_API void gameRender(GameState* gameState, Renderer* renderer, float dt){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    PROFILER_START();
    renderTileMap(renderer, gameState->bgMap, gameState->camera, 0.0f);
    systemRenderSprites(gameState, gameState->ecs, renderer, {ECS_TRANSFORM, ECS_SPRITE}, dt);
    systemRenderColliders(gameState, gameState->ecs, renderer, {ECS_2D_BOX_COLLIDER}, dt);
    renderTileMap(renderer, gameState->fgMap, gameState->camera, 1.0f);
    setYsort(renderer, false);
    PROFILER_END();
}

GAME_API void gameStop(){
    PROFILER_CLEANUP();
}