#include <malloc.h>

#include "game.hpp"
#include "tracelog.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


GAME_API void* gameStart(const char* testo){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    LOGINFO(testo);
    GameState* gameState = (GameState*)malloc(sizeof(GameState));

    Shader shader = createShader("shaders/default-shader.vs", "shaders/default-shader.fs");
    gameState->shader = shader;

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f,  0.5f, 0.0f,  // top left 
    };

    float vertices2[] = {
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    for(int i = 0; i < 9; i++){
        gameState->object[0].model.vertices.vertices[i] = vertices[i];
    }
    gameState->object[0].model.vertices.vertCount = 9;
    for(int i = 0; i < 9; i++){
        gameState->object[1].model.vertices.vertices[i] = vertices2[i];
    }
    gameState->object[1].model.vertices.vertCount = 9;

    gameState->object[0].pos = glm::vec3(0.0f, 0.0f, 0.0f);
    gameState->object[1].pos = glm::vec3(0.0f, 0.0f, 0.0f);

    gameState->transform = glm::mat4(1.0f);
    gameState->translate = glm::mat4(1.0f);

    return gameState;
}

GAME_API void gameUpdate(GameState* gameState, Input* input){
    if(input->keys[KEYS::Space]){
        LOGINFO("Sono il tasto spazio");
    }
    if(input->keys[KEYS::P]){
        LOGINFO("Sono il tasto P");
    }
    if(input->keys[KEYS::D]){
        LOGINFO("Sono il tasto D");
        gameState->transform = glm::translate(gameState->transform, glm::vec3(0.01f, 0.0f, 0.0f));
    }
}

GAME_API void gameRender(GameState* gameState, Renderer* renderer){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    setShader(renderer, gameState->shader);
    setUniform(&renderer->shader, "transform", gameState->transform);
    renderDraw(renderer, gameState->object[0].model.vertices);
    setUniform(&renderer->shader, "transform", gameState->transform);
    renderDraw(renderer, gameState->object[1].model.vertices);
}

GAME_API void gameStop(){
}