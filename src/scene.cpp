#include "scene.hpp"
#include "texture.hpp"
#include "ecs.hpp"

Scene createScene(Renderer* renderer){
    Scene scene = {};
    scene.ecs = initEcs();
    std::vector<ComponentType> types = {ECS_TRANSFORM, ECS_SPRITE};
    Texture* wall = loadTexture("assets/sprites/wall.jpg");
    Texture* awesome = loadTexture("assets/sprites/awesomeface.png");

    setUniform(&renderer->shader, "sprite", 0);
    TransformComponent transform = {};
    transform.position = glm ::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm ::vec3(45.0f, 45.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    SpriteComponent sprite = {};
    sprite.texture = wall;

    SpriteComponent sprite2 = {};
    sprite2.texture = awesome;

    std::vector<void*> components = {&transform, &sprite};
    std::vector<void*> components2 = {&transform, &sprite2};
    uint32_t player = createEntity(scene.ecs, types, std::vector<void*>());
    glm::vec3 position = glm ::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm ::vec3(45.0f, 45.0f , 0.0f);
    glm::vec3 rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    updateTranformers(scene.ecs, player, position, scale, rotation);
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 15; j++){
            uint32_t id = createEntity(scene.ecs, types, components);
            position = glm::vec3(50.0f * i, 50.0f * j, 0.0f);
            scale = glm::vec3(45.0f, 45.0f, 0.0f);
            updateTranformers(scene.ecs, id, position, scale, rotation);
            scene.ecs->components.sprite[id].texture = wall;
        }
    }
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 15; j++){
            uint32_t id = j + (i * 15)+1; //only id 0 out, so the player
            removeComponent(scene.ecs, id, types);
        }
    }
    return scene;
}

void renderScene(Renderer* renderer, Scene scene){
    //Rendering code da spostare probabilmente altrove
    //Renderizza tutti gli oggetti presenti nella scena
    // per ora e' statico, ma lo generalizziamo subito
    // probabilmente deve essere un system dell'ecs
    // che renderizza tutte le entity con uno sprite
    setShader(renderer, renderer->shader);
    glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

    std::vector<Entity> entities = view(scene.ecs, {ECS_TRANSFORM, ECS_SPRITE});
    for(int i = 0 ; i < entities.size(); i ++){
        uint32_t id = entities[i];
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, scene.ecs->components.transforms[id].position);
        transform = glm::scale(transform, scene.ecs->components.transforms[id].scale);
        setUniform(&renderer->shader, "projection", projection);
        setUniform(&renderer->shader, "model", transform);
        if(scene.ecs->components.sprite[i].texture){
            renderDraw(renderer, &scene.ecs->components.sprite[id]);
        }
    }
}

void updatePlayer(Input* input, uint32_t player, Ecs* ecs, float dt){
    float velocity = 300.0f;
    std::vector<Entity> entities = view(ecs, {ECS_TRANSFORM, ECS_SPRITE});
    if(input->keys[KEYS::W]){
        ecs->components.transforms[player].position.y -= velocity * dt;
    }if(input->keys[KEYS::S]){
        ecs->components.transforms[player].position.y += velocity * dt;
    }if(input->keys[KEYS::A]){
        ecs->components.transforms[player].position.x -= velocity * dt;
    }if(input->keys[KEYS::D]){
        ecs->components.transforms[player].position.x += velocity * dt;
    }
}

void updateScene(Input* input, Scene scene, float dt){
    updatePlayer(input, 0, scene.ecs, dt);
}