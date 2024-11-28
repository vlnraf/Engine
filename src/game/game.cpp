#include <malloc.h>

#include "game.hpp"
#include "tracelog.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


GAME_API Scene* gameStart(const char* testo){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    LOGINFO(testo);

    Scene* scene = (Scene*)malloc(sizeof(Scene));
    createScene(scene);

    return scene;
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
        //gameState->transform = glm::translate(gameState->transform, glm::vec3(0.01f, 0.0f, 0.0f));
    }
}

GAME_API void gameRender(GameState* gameState){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
}

GAME_API void gameStop(){
}