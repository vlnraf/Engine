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

    Shader shader = createShader("shaders/default-shader.vs", "shaders/default-shader.fs");
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 18; j++){
            scene->entities[i].model.vertices.vertices[j] = quad[j];
        }
        scene->entities[i].pos.x = i*50.0f;
        scene->entities[i].pos.y = i*50.0f;
        scene->entities[i].pos.z = 0.0f;
        scene->entities[i].shader = shader;
    }
}