#include <malloc.h>

#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

MyProfiler prof;


GAME_API GameState* gameStart(Renderer* renderer){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    PROFILER_SAVE("profiler.json");
    GameState* gameState = new GameState();

    //Hmm con malloc non posso utilizzare array dinamici gestiti dalla std
    //creo nuovamente una mia libreria di vettori dinamici?
    //Oppure sapendo che il gioco sara' 2D so gia' perfettamente quanti vertici avra'
    //ogni singola entita' e quindi uso array fissi?
    //pero' la scena potrebbe avere teoricamente infiniti oggetti
    //porco giuda ^^
    //Scene* scene = (Scene*)malloc(sizeof(Scene));
    //Scene* scene = new Scene();
    //Scene* scene = new Scene();
    gameState->scene = createScene(renderer);

    return gameState;
}

GAME_API void gameUpdate(GameState* gameState, Input* input, float dt){
    PROFILER_START();
    updateScene(input, &gameState->scene, dt);
    PROFILER_END();
}

GAME_API void gameRender(GameState* gameState, Renderer* renderer, float dt){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    PROFILER_START();
    renderScene(renderer, &gameState->scene, dt);
    PROFILER_END();
}

GAME_API void gameStop(){
    PROFILER_CLEANUP();
}