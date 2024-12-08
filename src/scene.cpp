#include "scene.hpp"


Scene createScene(Renderer* renderer){
    Scene scene = {};
    scene.ecs = initEcs();
    Texture* wall = loadTexture("assets/sprites/wall.jpg");
    Texture* awesome = loadTexture("assets/sprites/awesomeface.png");
    Texture* white = getWhiteTexture();
    Texture* tileSet = loadTexture("assets/sprites/tileset01.png");
    Texture* hero = loadTexture("assets/sprites/hero.png");
    Texture* treeSprite = loadTexture("assets/sprites/tree.png");

    TileSet simple = createTileSet(tileSet, 32);

    //for(int i = 0; i < scene.map.width; i++){
    //    for(int j = 0; j < scene.map.height; j++){
    //        scene.map.tiles.push_back(scene.map.tileset.tiles[0]);
    //    }
    //}
    std::vector<uint32_t> tileBg = {
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        };

    std::vector<uint32_t> tileFg = {
        349, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 351,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        369, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 371,
        389, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 390, 391,
        };

    scene.bgMap = createTilemap(tileBg, 20, 10, 32, simple);
    scene.fgMap = createTilemap(tileFg, 20, 10, 32, simple);

    TransformComponent transform = {};
    transform.position = glm ::vec3(10.0f, 10.0f, 0.0f);
    transform.scale = glm ::vec3(45.0f, 45.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 45.0f);

    SpriteComponent sprite = {};
    sprite.id = white->id;
    sprite.vertCount = QUAD_VERTEX_SIZE;

    InputComponent inputC = {};
    inputC.x = 0.0f;
    inputC.y = 0.0f;

    VelocityComponent velocity = {};
    velocity.x = 0.0f;
    velocity.y = 0.0f;

    
    //for(int i = 0; i < 70; i++){
    //    for(int j = 0; j < 30; j++){
    //        transform.position = glm::vec3(10.0f * i, 10.0f * j, 0.0f);
    //        transform.scale = glm::vec3(7.0f, 7.0f, 0.0f);
    //        uint32_t id = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    //        //pushComponent(scene.ecs, id, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
    //        sprite.id = wall->id;
    //        pushComponent(scene.ecs, id, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
    //    }
    //}

    scene.camera = createCamera(glm::vec3(0.0f, 0.0f, 0.0f), 640, 320);

    transform.position = glm ::vec3(32.0f, 32.0f, 0.0f);
    transform.scale = glm ::vec3(25.0f, 25.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    uint32_t player = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    //sprite.id = awesome->id;
    sprite.id = hero->id;
    pushComponent(scene.ecs, player, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
    pushComponent(scene.ecs, player, ECS_INPUT, (void*)&inputC, sizeof(InputComponent));
    pushComponent(scene.ecs, player, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
    scene.player = player;

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    transform.scale = glm ::vec3(64.0f, 160.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    uint32_t tree = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    sprite.id = treeSprite->id;
    sprite.vertCount = QUAD_VERTEX_SIZE;
    pushComponent(scene.ecs, tree, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));

    for(int i = 0; i < 10; i++){
        transform.position = glm ::vec3(i * 100, i * 50, 0.0f);
        transform.scale = glm ::vec3(10.0f, 10.0f , 0.0f);
        transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
        uint32_t enemy = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
        sprite.id = awesome->id;
        sprite.vertCount = QUAD_VERTEX_SIZE;
        EnemyComponent enemyComp = {};
        pushComponent(scene.ecs, enemy, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
        pushComponent(scene.ecs, enemy, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
        pushComponent(scene.ecs, enemy, ECS_ENEMY, (void*)&enemyComp, sizeof(EnemyComponent));
    }
    //removeEntity(scene.ecs, player);
    return scene;
}


void systemRender(Scene* scene, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types){
    std::vector<Entity> entities = view(ecs, types);
    setShader(renderer, renderer->shader);
    glm::mat4 projection = glm::ortho(0.0f, 640.0f, 0.0f, 320.0f, -100.0f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -scene->camera.position);
    for(int i = 0 ; i < entities.size(); i ++){
        glm::mat4 model = glm::mat4(1.0f);
        uint32_t id = entities[i];
        TransformComponent* t= (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, id, ECS_SPRITE);
        model = glm::translate(model, t->position);

        //In order to rotate the model from the center of the QUAD
        //model = glm::translate(model, glm::vec3(0.5f * t->scale.x, 0.5f * t->scale.y, 0.0f));
        //model = glm::rotate(model, glm::radians(t->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        //model = glm::translate(model, glm::vec3(-0.5f * t->scale.x, -0.5f * t->scale.y, 0.0f));

        //if(i == 0){
        //    float camPosX = floor(t->position.x - (640.0f / 2));
        //    float camPosY = floor(t->position.y - (320.0f / 2));
        //    view = glm::translate(glm::mat4(1.0f), -glm::vec3(camPosX, camPosY, 0.0f));
        //}

        model = glm::scale(model, t->scale);
        //view = glm::translate(view, t->position);

        setUniform(&renderer->shader, "projection", projection);
        setUniform(&renderer->shader, "model", model);
        setUniform(&renderer->shader, "view", view);
        setUniform(&renderer->shader, "layer", 1.0f + (1.0f - (t->position.y / 320.0f))); //1.0f is the "layer" and 320 the viewport height
        if(s->id >= 0){
            renderDraw(renderer, s->id, s->vertices, s->vertCount);
        }
    }
}

void moveSystem(Ecs* ecs, std::vector<ComponentType> types, float dt){
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
}

void inputSystem(Ecs* ecs, Input* input, std::vector<ComponentType> types){
    std::vector<Entity> entities = view(ecs, types);
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, id, ECS_VELOCITY);
        {   //GamePad
            if(input->gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER]){LOGINFO("Trigger Sinistro");}
            if(input->gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER]){LOGINFO("Trigger Destro");}
            if(abs(input->gamepad.leftX) > 0.1 || abs(input->gamepad.leftY) > 0.1){ //threshold because it's never 0.0
                vel->x = ((input->gamepad.leftX) * 100);
                vel->y = (-(input->gamepad.leftY) * 100);
            }

        //    //LOGINFO("left axis : %f - %f", input->gamepad.leftX, input->gamepad.leftY);
        //    //LOGINFO("right axis : %f - %f", input->gamepad.rightX, input->gamepad.rightY);
        }
        if(input->keys[KEYS::W]){ vel->y += 100.0f; }
        if(input->keys[KEYS::S]){ vel->y += -100.0f;  }
        if(input->keys[KEYS::A]){ vel->x += -100.0f; }
        if(input->keys[KEYS::D]){ vel->x += 100.0f;  }
    }

}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera, Entity id){
    TransformComponent* t = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);

    followTarget(camera, t->position);
}

void enemyFollowPlayerSystem(Ecs* ecs, Entity player, std::vector<ComponentType> types, float dt){
    std::vector<Entity> entities = view(ecs, types);
    TransformComponent* playerT = (TransformComponent*) getComponent(ecs, player, ECS_TRANSFORM);
    float dirX, dirY;
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, id, ECS_VELOCITY);
        TransformComponent* t = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);

        dirX = playerT->position.x - t->position.x;
        dirY = playerT->position.y - t->position.y;
        glm::vec3 dir = glm::normalize(glm::vec3(dirX, dirY, 0.0f));

        vel->x = 20.0f * dir.x * dt;
        vel->y = 20.0f * dir.y * dt;
        t->position += glm::vec3(vel->x, vel->y, 0.0f);
    }

}

void renderScene(Renderer* renderer, Scene* scene){
    renderTileMap(renderer, scene->bgMap, 0.0f);
    systemRender(scene, scene->ecs, renderer, {ECS_TRANSFORM, ECS_SPRITE});
    renderTileMap(renderer, scene->fgMap, 1.0f);
}
    
    

void updateScene(Input* input, Scene* scene, float dt){
    inputSystem(scene->ecs, input, {ECS_VELOCITY, ECS_INPUT});
    moveSystem(scene->ecs, {ECS_TRANSFORM, ECS_VELOCITY}, dt);
    cameraFollowSystem(scene->ecs, &scene->camera, scene->player);
    enemyFollowPlayerSystem(scene->ecs, scene->player, {ECS_VELOCITY, ECS_TRANSFORM, ECS_ENEMY}, dt);
}