#include "scene.hpp"
#include "texture.hpp"

void createScene(Scene* scene, Renderer* renderer){

    scene->ecs = initEcs();
    std::vector<ComponentType> types = {ECS_TRANSFORM, ECS_SPRITE};
    Texture* wall = loadTexture("assets/sprites/wall.jpg");
    Texture* awesome = loadTexture("assets/sprites/awesomeface.png");

    setUniform(&renderer->shader, "sprite", 0);
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 15; j++){
            createEntity(scene->ecs, types);
            scene->ecs->components.transforms[j+(i*15)].position = glm::vec3(50.0f * i, 50.0f * j, 0.0f);
            if(i == 0 && j == 0){
                scene->ecs->components.sprite[j+(i*15)].texture = awesome;
            }else{
                scene->ecs->components.sprite[j+(i*15)].texture = wall;
            }
        }
    }
    //LOGINFO("%d, %d",   scene->ecs->components.sprite[0].texture->width,
    //                    scene->ecs->components.sprite[0].texture->height);
}

void renderScene(Scene* scene, Renderer* renderer){
    //Rendering code da spostare probabilmente altrove
    //Renderizza tutti gli oggetti presenti nella scena
    // per ora e' statico, ma lo generalizziamo subito
    setShader(renderer, renderer->shader);
    glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    for(int i = 0 ; i < scene->ecs->entities; i ++){
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, scene->ecs->components.transforms[i].position);
        transform = glm::scale(transform, glm::vec3(45.0f, 45.0f, 0.0f));
        setUniform(&renderer->shader, "projection", projection);
        setUniform(&renderer->shader, "transform", transform);
        renderDraw(renderer, &scene->ecs->components.sprite[i]);
    }
}