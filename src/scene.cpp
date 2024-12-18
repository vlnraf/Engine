#include "scene.hpp"
#include "kit.hpp"

//struct AnimationManager{
//    std::unordered_map<char*, AnimationComponent> animations;
//};

//AnimationManager animationManager;

Scene createScene(Renderer* renderer){
    PROFILER_START();
    Scene scene = {};
    scene.ecs = initEcs();
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

    scene.bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    scene.fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    TransformComponent transform = {};
    transform.position = glm ::vec3(10.0f, 10.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 45.0f);

    SpriteComponent sprite = {};
    sprite.texture = white;

    InputComponent inputC = {};
    inputC.x = 0.0f;
    inputC.y = 0.0f;

    VelocityComponent velocity = {};
    velocity.x = 0.0f;
    velocity.y = 0.0f;

    AnimationComponent anim = {};

    scene.camera = createCamera(glm::vec3(0.0f, 0.0f, 0.0f), 640, 320);

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    //transform.scale = glm ::vec3(25.0f, 25.0f , 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f, 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    uint32_t player = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    //sprite.id = awesome->id;
    sprite.texture = idleWalk;
    sprite.index = {0,0};
    sprite.size = {16, 16};

    //AnimationManager* animationManager = &scene.animationManager;

    { //TODO: create an animationManager with functions to create animations
      //there is too much duplicated code :)
        anim.frames = 4;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,0};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"idleRight", anim});

        anim.frames = 4;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,0};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"idleLeft", anim});

        anim.frames = 4;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,1};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"idleBottom", anim});

        anim.frames = 4;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,2};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"idleTop", anim});

        anim.frames = 8;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,3};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"walkRight", anim});

        anim.frames = 8;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,3};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"walkLeft", anim});

        anim.frames = 8;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,4};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"walkBottom", anim});

        anim.frames = 8;
        for(int i = 0; i < anim.frames; i++){
            anim.indices[i] = {i,5};
        }
        anim.currentFrame = 0;
        anim.frameDuration = 1.0f / anim.frames;
        animationManager->animations.insert({"walkTop", anim});
    }


    //anim.frameDuration = 0.3;
    //anim.frames = {{0,0}, {1,0}, {2,0}, {3,0},{4,0},{5,0},{6,0}};
    pushComponent(scene.ecs, player, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
    pushComponent(scene.ecs, player, ECS_INPUT, (void*)&inputC, sizeof(InputComponent));
    pushComponent(scene.ecs, player, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
    pushComponent(scene.ecs, player, ECS_ANIMATION, (void*)&anim, sizeof(AnimationComponent));
    scene.player = player;

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    uint32_t tree = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    sprite.texture = treeSprite;
    sprite.index = {0,0};
    sprite.size = {treeSprite->width, treeSprite->height};
    pushComponent(scene.ecs, tree, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));

    srand(time(NULL));

    for(int i = 0; i < 3000; i++){
        transform.position = glm::vec3(rand() % 600 + 32, rand() % 300 + 32, 0.0f);
        transform.scale = glm ::vec3(0.02f, 0.02f , 0.0f);
        transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
        uint32_t enemy = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
        sprite.texture = awesome;
        EnemyComponent enemyComp = {};
        pushComponent(scene.ecs, enemy, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
        pushComponent(scene.ecs, enemy, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
        pushComponent(scene.ecs, enemy, ECS_ENEMY, (void*)&enemyComp, sizeof(EnemyComponent));
    }
    //removeEntity(scene.ecs, player);
    PROFILER_END();
    return scene;
}


void systemRender(Scene* scene, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    setShader(renderer, renderer->shader);

    for(int i = 0 ; i < entities.size(); i ++){
        uint32_t id = entities[i];
        TransformComponent* t= (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, id, ECS_SPRITE);
        AnimationComponent* anim= (AnimationComponent*) getComponent(ecs, id, ECS_ANIMATION);
        setUniform(&renderer->shader, "layer", 1.0f + (1.0f - (t->position.y / 320.0f))); //1.0f is the "layer" and 320 the viewport height
        if(!anim){
            if(s->texture){
                //renderDrawQuad(renderer, scene->camera, t->position, t->scale, t->rotation, s->texture, s->index, s->size);
                renderDrawQuad(renderer, scene->camera, t->position, t->scale, t->rotation, s->texture);
            }
        }else{
            anim->frameCount += dt;
            //renderDrawQuad(renderer, scene->camera, t->position, t->scale, t->rotation, s->texture, {anim->currentFrame, 0}, s->size);
            renderDrawQuad(renderer, scene->camera, t->position, t->scale, t->rotation, s->texture, anim->frames[anim->currentFrame], s->size);
            if(anim->frameCount >= anim->frameDuration){
                anim->currentFrame = (anim->currentFrame + 1) % (anim->framesSize); // module to loop around
                anim->frameCount = 0;
            }
        }
    }
    PROFILER_END();
}

void moveSystem(Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, id, ECS_VELOCITY);
        transform->position.x += vel->x * dt;
        transform->position.y += vel->y * dt;
        vel->x = 0.0f;
        vel->y = 0.0f;

        transform->rotation.z += (dt * 100.0f);
    }
    PROFILER_END();
}

void inputSystem(Scene* scene, Ecs* ecs, Input* input, std::vector<ComponentType> types){
    PROFILER_START();
    //NOTE: this cause dll to fail sometimes to load because access to some pointer result in corrupted memory
    //just make it works with the dll
    //probably it's the char* to identify animations that got corrupted
    //i am sure it is, because with string the problems seems to be gone
    AnimationManager* animationManager = &scene->animationManager;
    std::vector<Entity> entities = view(ecs, types);
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, id, ECS_VELOCITY);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, id, ECS_SPRITE);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);
        {   //GamePad
            AnimationComponent* data = &animationManager->animations.at("idleRight");
            if(input->gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER]){LOGINFO("Trigger Sinistro");}
            if(input->gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER]){LOGINFO("Trigger Destro");}
            if(abs(input->gamepad.leftX) > 0.1 || abs(input->gamepad.leftY) > 0.1){ //threshold because it's never 0.0
                //NOTE: Input entity can have no animation component, error prone
                //TODO: just implement the change of animation logic elsewhere
                //I think the input system is not the system where do this logic
                //And implement direction logic instead of threshold
                if(input->gamepad.leftX < -0.3){
                    transform->scale.x = -1.0f;
                    data = &animationManager->animations.at("walkLeft");
                }else if(input->gamepad.leftX > 0.3){
                    //transform->position.x = transform->position.x + sprite->texture->width;
                    transform->scale.x = 1.0f;
                    data = &animationManager->animations.at("walkRight");
                }else if(input->gamepad.leftY > 0.3){
                    data = &animationManager->animations.at("walkTop");
                }else if(input->gamepad.leftY < -0.3){
                    data = &animationManager->animations.at("walkBottom");
                }
                vel->x = ((input->gamepad.leftX) * 100);
                vel->y = ((input->gamepad.leftY) * 100);
            }
            setComponent(ecs,id, data, ECS_ANIMATION);

            //LOGINFO("left axis : %f / %f", input->gamepad.leftX, input->gamepad.leftY);
        }
        if(input->keys[KEYS::W]){ vel->y += 100.0f; }
        if(input->keys[KEYS::S]){ vel->y += -100.0f;  }
        if(input->keys[KEYS::A]){ vel->x += -100.0f; }
        if(input->keys[KEYS::D]){ vel->x += 100.0f;  }
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
    float dirX, dirY;
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, id, ECS_VELOCITY);
        TransformComponent* t = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);

        dirX = followPlayer.x - t->position.x;// (t->position.x + (0.5 * t->scale.x));
        dirY = followPlayer.y - t->position.y;
        glm::vec3 dir = glm::normalize(glm::vec3(dirX, dirY, 0.0f));

        vel->x = 10.0f * dir.x * dt;
        vel->y = 10.0f * dir.y * dt;
        t->position += glm::vec3(vel->x, vel->y, 0.0f);
    }
    PROFILER_END();

}

void updateScene(Input* input, Scene* scene, float dt){
    PROFILER_START();
    inputSystem(scene, scene->ecs, input, {ECS_TRANSFORM, ECS_VELOCITY, ECS_INPUT});
    moveSystem(scene->ecs, {ECS_TRANSFORM, ECS_VELOCITY}, dt);
    cameraFollowSystem(scene->ecs, &scene->camera, scene->player);
    enemyFollowPlayerSystem(scene->ecs, scene->player, {ECS_VELOCITY, ECS_TRANSFORM, ECS_ENEMY}, dt);
    PROFILER_END();
}

void renderScene(Renderer* renderer, Scene* scene, float dt){
    PROFILER_START();
    renderTileMap(renderer, scene->bgMap, scene->camera, 0.0f);
    systemRender(scene, scene->ecs, renderer, {ECS_TRANSFORM, ECS_SPRITE}, dt);
    renderTileMap(renderer, scene->fgMap, scene->camera, 0.5f);
    PROFILER_END();
}