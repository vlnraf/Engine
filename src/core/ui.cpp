#include "ui.hpp"
#include "input.hpp"
#include "renderer/renderer.hpp"

static UIState* uiState;

void initUI(glm::vec2 screenSize){
    uiState = new UIState();
    uiState->screenSize = screenSize;
}

void destroyUI(){
    delete uiState;
}

void beginUIFrame(glm::vec2 canvasPos, glm::vec2 canvasSize){
    uiState->canvasSize = canvasSize;
    uiState->mousePos = getMousePos();
    glm::vec2 scale = uiState->canvasSize / uiState->screenSize;
    uiState->mousePos = uiState->mousePos * scale;

    beginUIRender(canvasPos, canvasSize);
    uiState->id = 1;
    uiState->hot = 0;
    uiState->active = 0;
}

void endUIFrame(){
    endUIRender();
}

bool pointRectIntersection(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size){
    return  mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y;
}

bool UIButton(const char* fmt, glm::vec2 pos, glm::vec2 size, glm::vec3 rotation){
    glm::vec2 mousePos = uiState->mousePos;
    //glm::vec2 scale = {640.0f / (float)screenWidth, 320.0f / (float)screenHeight};
    //mousePos = mousePos * scale;
    //LOGINFO("%f / %f", mousePos.x, mousePos.y);
    uint32_t buttonId = uiState->id++;

    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + size.y)};

    glm::vec4 color = {};

    if(pointRectIntersection(mousePos, pos, size)){
        uiState->hot = buttonId;
        color =  {1.0f, 0.0f, 0.0f, 0.5f};
    }else{
        uiState->hot = 0;
        color =  {0.0f, 0.0f, 0.0f, 0.5f};
    }
    
    renderDrawFilledRectUI(screenPos, size, rotation, color);
    renderDrawTextUI(fmt, screenPos.x, screenPos.y, 1.0f);
    return false;
}