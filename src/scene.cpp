#include "scene.hpp"
#include "texture.hpp"
#include "ecs.hpp"

Scene createScene(Renderer* renderer){
    Scene scene = {};
    scene.ecs = initEcs();
    Texture* wall = loadTexture("assets/sprites/wall.jpg");
    Texture* awesome = loadTexture("assets/sprites/awesomeface.png");

    TransformComponent transform = {};
    transform.position = glm ::vec3(10.0f, 10.0f, 0.0f);
    transform.scale = glm ::vec3(45.0f, 45.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);

    SpriteComponent sprite = {};
    sprite.texture = getWhiteTexture();
    sprite.vertCount = QUAD_VERTEX_SIZE;

    InputComponent inputC = {};
    inputC.x = 0.0f;
    inputC.y = 0.0f;

    VelocityComponent velocity = {};
    velocity.x = 0.0f;
    velocity.y = 0.0f;

    uint32_t player = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
    pushComponent(scene.ecs, player, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
    pushComponent(scene.ecs, player, ECS_INPUT, (void*)&inputC, sizeof(InputComponent));
    pushComponent(scene.ecs, player, ECS_VELOCITY, (void*)&velocity, sizeof(VelocityComponent));
    
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 15; j++){
            transform.position = glm::vec3(50.0f * i, 50.0f * j, 0.0f);
            transform.scale = glm::vec3(45.0f, 45.0f, 0.0f);
            uint32_t id = createEntity(scene.ecs, ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
            sprite.texture = wall;
            pushComponent(scene.ecs, id, ECS_SPRITE, (void*)&sprite, sizeof(SpriteComponent));
        }
    }
    //removeEntity(scene.ecs, player);
    return scene;
}


void systemRender(Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types){
    std::vector<Entity> entities = view(ecs, types);
    setShader(renderer, renderer->shader);
    glm::mat4 projection = glm::ortho(0.0f, (float)renderer->width, (float)renderer->height, 0.0f, -1.0f, 1.0f);
    for(int i = 0 ; i < entities.size(); i ++){
        glm::mat4 transform = glm::mat4(1.0f);
        uint32_t id = entities[i];
        TransformComponent* t= (TransformComponent*) ecs->components[ECS_TRANSFORM][id].data;
        SpriteComponent * s = (SpriteComponent*) ecs->components[ECS_SPRITE][id].data;
        transform = glm::translate(transform, t->position);
        transform = glm::scale(transform, t->scale);
        setUniform(&renderer->shader, "projection", projection);
        setUniform(&renderer->shader, "model", transform);
        if(s->texture){
            renderDraw(renderer, s->texture, s->vertices, s->vertCount);
        }
    }
}

void moveSystem(Ecs* ecs, std::vector<ComponentType> types, float dt){
    std::vector<Entity> entities = view(ecs, types);
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        TransformComponent* transform = (TransformComponent*) ecs->components[ECS_TRANSFORM][id].data;
        VelocityComponent* vel = (VelocityComponent*) ecs->components[ECS_VELOCITY][id].data;
        transform->position.x += vel->x * dt;
        transform->position.y += vel->y * dt;
        vel->x = 0.0f;
        vel->y = 0.0f;
    }
}

void inputSystem(Ecs* ecs, Input* input, std::vector<ComponentType> types){
    std::vector<Entity> entities = view(ecs, types);
    for(int i = 0; i < entities.size(); i++){
        uint32_t id = entities[i];
        VelocityComponent* vel = (VelocityComponent*) ecs->components[ECS_VELOCITY][id].data;
        if(input->keys[KEYS::W]){ vel->y = -100.0f; }
        if(input->keys[KEYS::S]){ vel->y = 100.0f;  }
        if(input->keys[KEYS::A]){ vel->x = -100.0f; }
        if(input->keys[KEYS::D]){ vel->x = 100.0f;  }
    }

}

void renderScene(Renderer* renderer, Scene scene){
    //Rendering code da spostare probabilmente altrove
    //Renderizza tutti gli oggetti presenti nella scena
    // per ora e' statico, ma lo generalizziamo subito
    // probabilmente deve essere un system dell'ecs
    // che renderizza tutte le entity con uno sprite
    systemRender(scene.ecs, renderer, {ECS_TRANSFORM, ECS_SPRITE});
}

void updateScene(Input* input, Scene scene, float dt){
    inputSystem(scene.ecs, input, {ECS_VELOCITY, ECS_INPUT});
    moveSystem(scene.ecs, {ECS_TRANSFORM, ECS_SPRITE, ECS_INPUT, ECS_VELOCITY}, dt);
}