#include "ui.hpp"
#include "input.hpp"
#include "renderer/renderer.hpp"

#include "core.hpp"

static UIState* uiState;

UIState* initUI(glm::vec2 screenSize){
    uiState = new UIState();
    uiState->screenSize = screenSize;
    uiState->id = 1;
    uiState->hot = 0;
    uiState->active = 0;
    uiState->font = getFont("Minecraft");
    return uiState;
}

void setFontUI(Font* font){
    uiState->font = font;
}

void destroyUI(){
    delete uiState;
}

std::vector<std::string> splitText(const char* text, const char separator){
    std::vector<std::string> result;
    //char word[100];
    std::string word;
    int j = 0;
    for(int i = 0; text[i] != '\0'; i++){
        if(text[i] != separator){
            word[j] = text[i];
            j++;
        }else{
            word[j] = '\0';
            result.push_back(word);
            //memset(word, 0, sizeof(char)*100);
            //i++;
            j=0;
        }
    }
    return result;
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

    //uint32_t textWidth = calculateTextWidth(uiState->font, text, 0.3f);
    //uint32_t totalSize = 0;
    //std::vector<std::string> lines;
    //std::string line = "";
    //if(textWidth > (uint32_t)size.x){
    //    std::vector<std::string> words = splitText(text, ' ');
    //    for(size_t i = 0; i < words.size(); i++){
    //        totalSize += calculateTextWidth(uiState->font, words[i].c_str(), 0.3f);
    //        if(totalSize < (uint32_t)size.x){
    //            //strcpy(line.c_str(), words[i].c_str());
    //            //line.append(words[i]);
    //            line = line + words[i].c_str();
    //            lines.push_back(line);
    //        }else{
    //            lines.push_back(line);
    //        }
    //    }
    //}
    renderDrawFilledRect(screenPos, size, rotation, color);
    UiText(text, pos, 0.3f);
    //for(size_t i = 0; i < lines.size(); i++){
    //    pos.y = pos.y + (i * uiState->font->characters->Size.y * 0.3f);
    //    UiText(lines[i].c_str(), pos, 0.3f);
    //}
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

int UigetTextHeight(const char* text, float scale){
    int result = 0;
    for(int i = 0; text[i] != '\0'; i++){
        int newResult = uiState->font->characters->Size.y * scale;
        if(newResult > result){
            result = newResult;
        }
    }
    return result;
}
