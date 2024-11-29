#include "scene.hpp"

void createScene(Scene* scene){
    float quad[] = {
        // pos      
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    scene->ecs = initEcs(10);
    std::vector<ComponentType> types = {ECS_TRANSFORM};
    std::vector<ComponentType> types2 = {ECS_TRANSFORM, ECS_INPUT};
    createEntity(scene->ecs, types2);
    createEntity(scene->ecs, types2);
    createEntity(scene->ecs, types);
    Shader shader = createShader("shaders/default-shader.vs", "shaders/default-shader.fs");

    for(int i = 0; i < 10; i++){
        //scene->entities[i].vertices.reserve(18);
        for(int j = 0; j < 18; j++){
            //scene->entities[i].vertices.push_back(quad[j]);
            scene->entities[i].vertices[j] = quad[j];
        }
        scene->entities[i].pos.x = i*50.0f;
        scene->entities[i].pos.y = i*50.0f;
        scene->entities[i].pos.z = 0.0f;
        scene->entities[i].shader = shader;
        scene->entities[i].vertCount = QUAD_VERTEX_SIZE;
    }
}