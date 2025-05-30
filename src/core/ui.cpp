#include "ui.hpp"
#include "input.hpp"
#include "renderer/renderer.hpp"

#include "core.hpp"

static UIState* uiState;

void initUI(glm::vec2 screenSize){
    uiState = new UIState();
    uiState->screenSize = screenSize;
    uiState->id = 1;
    uiState->hot = 0;
    uiState->active = 0;
    uiState->font = getFont("Minecraft");
}

void destroyUI(){
    delete uiState;
}

void beginUiFrame(glm::vec2 canvasPos, glm::vec2 canvasSize){
    uiState->canvasSize = canvasSize;
    uiState->mousePos = getMousePos();
    glm::vec2 scale = uiState->canvasSize / uiState->screenSize;
    uiState->mousePos = uiState->mousePos * scale;
    uiState->mousePos.y = uiState->canvasSize.y - uiState->mousePos.y;

    //beginUIRender(canvasPos, canvasSize);
    OrtographicCamera uiCamera = createCamera({canvasPos.x,canvasPos.y,0}, canvasSize.x, canvasSize.y);
    beginScene(uiCamera, RenderMode::NO_DEPTH);
    uiState->id = 1;
    uiState->hot = 0;
    //uiState->active = 0;
}

void endUiFrame(){
    //endUIRender();
    endScene();
}

bool pointRectIntersection(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size){
    return  mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y;
}

void UiSetHot(uint32_t buttonId){
    if(uiState->active == 0 || uiState->active == buttonId){
        uiState->hot = buttonId;
    }
}

bool UiButton(const char* text, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation){
    bool result = false;
    glm::vec2 mousePos = uiState->mousePos;
    //glm::vec2 scale = {640.0f / (float)screenWidth, 320.0f / (float)screenHeight};
    //mousePos = mousePos * scale;
    //LOGINFO("%f / %f", mousePos.x, mousePos.y);
    uint32_t buttonId = uiState->id++;

    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + size.y)};

    glm::vec4 color =  {0.0f, 0.0f, 0.0f, 0.5f};


    if(pointRectIntersection(mousePos, screenPos, size)){
        UiSetHot(buttonId);
    }
    if(uiState->active == buttonId){
        if(isMouseButtonRelease(MOUSE_BUTTON_1)){
            if(uiState->hot == buttonId){
                result = true;
            }
            uiState->active = 0;
        }
        color =  {1.0f, 0.0f, 0.0f, 0.5f};
    }else if(uiState->hot == buttonId){
        if(isMouseButtonJustPressed(MOUSE_BUTTON_1)){
            uiState->active = buttonId;
        }
        color =  {1.0f, 1.0f, 0.0f, 0.5f};
    }

    renderDrawFilledRect(screenPos, size, rotation, color);
    UiText(text, pos, 1.0f);
    return result;
}

void UiText(const char* text, glm::vec2 pos, float scale){
    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + (uiState->font->characters->Size.y * scale))};
    renderDrawText2D(uiState->font, text, screenPos, scale);
}

void UiImage(Texture* texture, glm::vec2 pos, glm::vec2 rotation){
    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + texture->height)};
    renderDrawQuad2D(texture, screenPos, {1,1}, rotation, {0,0}, {texture->width, texture->height});
}

void UiImage(Texture* texture, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation, glm::vec2 index, glm::vec2 offset){
    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + texture->height)};
    renderDrawQuad2D(texture, screenPos, size, rotation, index, offset);
}