#include <malloc.h>

#include "game.hpp"
#include "tracelog.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


GAME_API Scene* gameStart(const char* testo, Renderer* renderer){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    LOGINFO(testo);

    //Hmm con malloc non posso utilizzare array dinamici gestiti dalla std
    //creo nuovamente una mia libreria di vettori dinamici?
    //Oppure sapendo che il gioco sara' 2D so gia' perfettamente quanti vertici avra'
    //ogni singola entita' e quindi uso array fissi?
    //pero' la scena potrebbe avere teoricamente infiniti oggetti
    //porco giuda ^^
    //Scene* scene = (Scene*)malloc(sizeof(Scene));
    //Scene* scene = new Scene();
    Scene* scene = new Scene();
    createScene(scene, renderer);

    return scene;
}

GAME_API void gameUpdate(Scene* gameState, Input* input){
    if(input->keys[KEYS::Space]){
        LOGINFO("Sono il tasto spazio");
    }
    if(input->keys[KEYS::P]){
        LOGINFO("Sono il tasto P");
    }
    if(input->keys[KEYS::D]){
        LOGINFO("Sono il tasto D");
    }
}

GAME_API void gameRender(Scene* gameState, Renderer* renderer){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    renderScene(gameState, renderer);
}

GAME_API void gameStop(){
}